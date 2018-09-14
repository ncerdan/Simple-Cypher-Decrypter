#ifndef MYHASH_G
#define MYHASH_G

//class used for simple linked list in each bucket
template <class KeyType, class ValueType>
class Node		
{
public:
	//toBeSetToMNext is the pointer from the correct bucket. If its empty its nullptr,
	//if not the new Node is 'inserted' before the other Nodes	
	Node(KeyType key, ValueType val, Node* toBeSetToMNext)
		:m_key(key), m_val(val), m_next(toBeSetToMNext)	  
	{}										
	//standard node structure
	KeyType m_key;		
	ValueType m_val;	
	Node* m_next;		
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
	
	//points to array which points to Nodes
	Node<KeyType, ValueType>** m_bucketsOfHeads;	

	unsigned int getBucketNumber(const KeyType& key) const
	{
		//prototype
		unsigned int hash(const KeyType& k);		
		unsigned int h = hash(key) % m_numBuckets;
		return h;
	}
};

//O(B)
template <class KeyType, class ValueType>
MyHash<KeyType, ValueType>::MyHash(double maxLoadFactor) 
	: m_numBuckets(100), m_numItems(0), m_maxLoadFactor(maxLoadFactor)
{
	// check and fix potential bad entries
	if (m_maxLoadFactor <= 0)		
		m_maxLoadFactor = 0.5;		
	if (m_maxLoadFactor > 2)		
		m_maxLoadFactor = 2.0;		
	
	//allocate a new array of length 100
	m_bucketsOfHeads = new Node<KeyType, ValueType>*[100];	
	
	// set each bucket to hold a nullptr bc its empty
	for (int i = 0; i < m_numBuckets; i++)	
		m_bucketsOfHeads[i] = nullptr;		
}

//O(B)
template <class KeyType, class ValueType>
MyHash<KeyType, ValueType>::~MyHash()
{
	//go through each bucket
	for (int i = 0; i < m_numBuckets; i++)	
	{
		// go through the list and delete all Nodes
		Node<KeyType, ValueType>* n = m_bucketsOfHeads[i];	
		while (n != nullptr)								
		{													
			Node<KeyType, ValueType>* killer = n;			
			n = n->m_next;									
			delete killer;									
		}													
	}
	//delete array
	delete[] m_bucketsOfHeads;
}

//O(B)
template <class KeyType, class ValueType>
void MyHash<KeyType, ValueType>::reset()
{
	//same as ~MyHash()
	for (int i = 0; i < m_numBuckets; i++)					
	{														
		Node<KeyType, ValueType>* n = m_bucketsOfHeads[i];	
		while (n != nullptr)								
		{													
			Node<KeyType, ValueType>* killer = n;			
			n = n->m_next;									
			delete killer;									
		}													
	}														

	//if the table grew beyond 100 buckets, create a new dynamically allocated array of 100 and delete old one
	if (m_numBuckets != 100)																
	{																						
		Node<KeyType, ValueType>** toBeSetToBuckets = new Node<KeyType, ValueType>*[100];	
		delete[] m_bucketsOfHeads;															
		m_bucketsOfHeads = toBeSetToBuckets;												
		m_numBuckets = 100;																	
	}																						

	//set each bucket to be empty
	for (int i = 0; i < m_numBuckets; i++)				
	{													
		m_bucketsOfHeads[i] = nullptr;					
	}													

	//reset item number member
	m_numItems = 0;		
}

//O(1) / O(X) / O(B) depending on whether it needs new dynamic array
template <class KeyType, class ValueType>
void MyHash<KeyType, ValueType>::associate(const KeyType& key, const ValueType& val)
{
	//if the key already exists, change its val and return
	if (find(key) != nullptr)	
	{
		*find(key) = val;
		return;
	}
	//else, create a new Node at the correct bucket, with correct m_next, and increment numItems
	else						
	{
		m_bucketsOfHeads[getBucketNumber(key)] = new Node<KeyType, ValueType>(key, val, m_bucketsOfHeads[getBucketNumber(key)]);
		m_numItems++;
	}

	//if we need to resize dynamic array
	if (getLoadFactor() > m_maxLoadFactor)	
	{
		//save the old number of buckets for for-loop, update member to double length
		int oldNumBuckets = m_numBuckets;	
		m_numBuckets *= 2;					

		//dynamically allocate a new and empty array of double the size
		Node<KeyType, ValueType>** toBeSetToBuckets = new Node<KeyType, ValueType>*[m_numBuckets];	
		for (int i = 0; i < m_numBuckets; i++)														
			toBeSetToBuckets[i] = nullptr;															

		//for each bucket in current m_bucketsOfHeads...
		for (int i = 0; i < oldNumBuckets; i++)					
		{											
			//and for each Node in each bucket
			Node<KeyType, ValueType>* n = m_bucketsOfHeads[i];	
			while (n != nullptr)								
			{
				//set this variable before changing anything	
				Node<KeyType, ValueType>* nextNodeToCheck = n->m_next;
				//set n's new m_next to its new bucket index's pointing value
				n->m_next = toBeSetToBuckets[getBucketNumber(n->m_key)];	
				//set the bucket at that index to point to the newly moved node
				toBeSetToBuckets[getBucketNumber(n->m_key)] = n;		
				//increment the traversing node pointer to the next one (it will become nullptr if its the last one)
				n = nextNodeToCheck;										
			}
		}

		//delete the dynamically allocated array
		delete[] m_bucketsOfHeads;				
		//set the array pointer to point to newly made array
		m_bucketsOfHeads = toBeSetToBuckets;	
	}
}

//O(1)/O(X)
template <class KeyType, class ValueType>
const ValueType* MyHash<KeyType, ValueType>::find(const KeyType& key) const
{	
	//find the linked list the key would belong in
	Node<KeyType, ValueType>* n = m_bucketsOfHeads[getBucketNumber(key)];	
	//go through each node in the list
	while (n != nullptr)						
	{											
		//if you find the key, return its reference
		if (n->m_key == key)					
			return &(n->m_val);		
		//if you dont move to next node
		n = n->m_next;							
	}										
	//if you reach the end without finding it, return nullptr
	return nullptr;		
}

template <class KeyType, class ValueType>
int MyHash<KeyType, ValueType>::getNumItems() const { return m_numItems; }

template <class KeyType, class ValueType>
double MyHash<KeyType, ValueType>::getLoadFactor() const { return ((static_cast<double>(m_numItems)) / static_cast<double>(m_numBuckets)); }

#endif