#ifndef MYHASH_G
#define MYHASH_G

template <class KeyType, class ValueType>
class Node		//class used for simple linked list in each bucket
{
public:
	Node(KeyType key, ValueType val, Node* toBeSetToMNext)
		:m_key(key), m_val(val), m_next(toBeSetToMNext)	//toBeSetToMNext is the pointer from the correct bucket. If its empty its nullptr,  
	{}													//if not the new Node is 'inserted' before the other Nodes													
	KeyType m_key;		//standard node structure
	ValueType m_val;	//
	Node* m_next;		//
};


template <class KeyType, class ValueType>
class MyHash
{
public:
	MyHash(double maxLoadFactor = 0.5);
	~MyHash();
	void reset();
	void associate(const KeyType& key, const ValueType& value);
	const ValueType* find(const KeyType& key) const;
	ValueType* find(const KeyType& key)
	{
		return const_cast<ValueType*>(const_cast<const MyHash*>(this)->find(key));
	}
	int getNumItems() const;
	double getLoadFactor() const;
private:
	int m_numBuckets;
	int m_numItems;
	double m_maxLoadFactor;
	
	Node<KeyType, ValueType>** m_bucketsOfHeads;	//points to array which points to Nodes

	unsigned int getBucketNumber(const KeyType& key) const
	{
		unsigned int hash(const KeyType& k);		//prototype
		unsigned int h = hash(key) % m_numBuckets;
		return h;
	}
};

template <class KeyType, class ValueType>
MyHash<KeyType, ValueType>::MyHash(double maxLoadFactor)	//FINISHED. has correct O(B)
	: m_numBuckets(100), m_numItems(0), m_maxLoadFactor(maxLoadFactor)
{
	if (m_maxLoadFactor <= 0)		// check and fix potential bad entries
		m_maxLoadFactor = 0.5;		//
	if (m_maxLoadFactor > 2)		//
		m_maxLoadFactor = 2.0;		//
	
	m_bucketsOfHeads = new Node<KeyType, ValueType>*[100];	//allocate a new array of length 100
	
	for (int i = 0; i < m_numBuckets; i++)	// set each bucket to hold a nullptr bc its empty
		m_bucketsOfHeads[i] = nullptr;		//
}

template <class KeyType, class ValueType>
MyHash<KeyType, ValueType>::~MyHash()	// FINSIHED. has correct O(B)
{
	for (int i = 0; i < m_numBuckets; i++)	//go through each bucket
	{
		Node<KeyType, ValueType>* n = m_bucketsOfHeads[i];	// go through the list and delete all Nodes
		while (n != nullptr)								//
		{													//
			Node<KeyType, ValueType>* killer = n;			//
			n = n->m_next;									//
			delete killer;									//
		}													//
	}
	delete[] m_bucketsOfHeads;			//delete array
}

template <class KeyType, class ValueType>
void MyHash<KeyType, ValueType>::reset()	//FINISHED. has correct O(B)
{
	for (int i = 0; i < m_numBuckets; i++)					//same as ~MyHash()
	{														//
		Node<KeyType, ValueType>* n = m_bucketsOfHeads[i];	//
		while (n != nullptr)								//
		{													//
			Node<KeyType, ValueType>* killer = n;			//
			n = n->m_next;									//
			delete killer;									//
		}													//
	}														//

	if (m_numBuckets != 100)																//if the table grew beyond 100 buckets, create a new dynamically allocated array of 100 and delete old one
	{																						//
		Node<KeyType, ValueType>** toBeSetToBuckets = new Node<KeyType, ValueType>*[100];	//
		delete[] m_bucketsOfHeads;															//
		m_bucketsOfHeads = toBeSetToBuckets;												//
		m_numBuckets = 100;																	//
	}																						//

	for (int i = 0; i < m_numBuckets; i++)				//sets each bucket to be empty
	{													//
		m_bucketsOfHeads[i] = nullptr;					//
	}													//

	m_numItems = 0;		//reset item number member
}

template <class KeyType, class ValueType>
void MyHash<KeyType, ValueType>::associate(const KeyType& key, const ValueType& val)	// FINIHSED. has correct O(1)/O(X)/O(B) depending on whether it needs new dynamic array
{
	if (find(key) != nullptr)	//if the key already exists, change its val and return
	{
		*find(key) = val;
		return;
	}
	else						//else, create a new Node at the correct bucket, with correct m_next, and increment numItems
	{
		m_bucketsOfHeads[getBucketNumber(key)] = new Node<KeyType, ValueType>(key, val, m_bucketsOfHeads[getBucketNumber(key)]);
		m_numItems++;
	}

	if (getLoadFactor() > m_maxLoadFactor)	//if we need to resize dynamic array
	{
		int oldNumBuckets = m_numBuckets;	//save the old number of buckets for for-loop, update member to double length
		m_numBuckets *= 2;					//

		Node<KeyType, ValueType>** toBeSetToBuckets = new Node<KeyType, ValueType>*[m_numBuckets];	//dynamically allocate a new and empty array of double the size
		for (int i = 0; i < m_numBuckets; i++)														//
			toBeSetToBuckets[i] = nullptr;															//

		for (int i = 0; i < oldNumBuckets; i++)					//for each bucket in current m_bucketsOfHeads,
		{														//
			Node<KeyType, ValueType>* n = m_bucketsOfHeads[i];	//and for each Node in each bucket
			while (n != nullptr)								//
			{
				Node<KeyType, ValueType>* nextNodeToCheck = n->m_next;		//set this variable before changing anything		
				n->m_next = toBeSetToBuckets[getBucketNumber(n->m_key)];	//set n's new m_next to its new bucket index's pointing value
				toBeSetToBuckets[getBucketNumber(n->m_key)] = n;			//set the bucket at that index to point to the newly moved node
				n = nextNodeToCheck;										//increment the traversing node pointer to the next one (it will become nullptr if its the last one)
			}
		}

		delete[] m_bucketsOfHeads;				//delete the dynamically allocated array
		m_bucketsOfHeads = toBeSetToBuckets;	//set the array pointer to point to newly made array
	}
}

template <class KeyType, class ValueType>
const ValueType* MyHash<KeyType, ValueType>::find(const KeyType& key) const	//FINISHED. has correct O(1)/O(X)
{	
	Node<KeyType, ValueType>* n = m_bucketsOfHeads[getBucketNumber(key)];	//find the linked list the key would belong in
	while (n != nullptr)						//go through each node in the list
	{											//
		if (n->m_key == key)					//if you find the key, return its reference
			return &(n->m_val);					//
		n = n->m_next;							//if you dont move to next node
	}											//
	return nullptr;		//if you reach the end without finding it, return nullptr
}

template <class KeyType, class ValueType>
int MyHash<KeyType, ValueType>::getNumItems() const { return m_numItems; }

template <class KeyType, class ValueType>
double MyHash<KeyType, ValueType>::getLoadFactor() const { return ((static_cast<double>(m_numItems)) / static_cast<double>(m_numBuckets)); }

#endif