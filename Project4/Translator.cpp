#include "provided.h"
#include "MyHash.h"
#include <list>

class TranslatorImpl
{
public:
	TranslatorImpl();
	~TranslatorImpl();
	bool pushMapping(std::string ciphertext, std::string plaintext);
	bool popMapping();
	std::string getTranslation(const std::string& ciphertext) const;
private:
	bool isInconsistent(char cipherLetter, char plainLetter) const;

	MyHash<char, char>* m_currCtoPMapping;
	MyHash<char, char>* m_currPtoCMapping;
	//KeyType	of char is for lowercase ciphertext/plaintext letter key
	//ValueType of char is for lowercase plaintext/ciphertext letter its key corresponds to

	//implementing one stack for each mapping as lists by only using push_front() and pop_front()
	std::list<MyHash<char, char>*> m_myStackOfCtoPMappings;		
	std::list<MyHash<char, char>*> m_myStackOfPtoCMappings;			

	//to ensure I don't try to pop when it's empty
	int m_timesPushed;	
	int m_timesPopped;	
};

TranslatorImpl::TranslatorImpl()
	: m_timesPushed(0), m_timesPopped(0)
{
	//dynamically allocate both hashtables
	m_currCtoPMapping = new MyHash<char, char>;		
	m_currPtoCMapping = new MyHash<char, char>;		

	//sets both mappings to a mapping from each letter a-z to '?'
	for (int i = 0; i < 26; i++)					
	{												
		m_currCtoPMapping->associate('a' + i, '?');	
		m_currPtoCMapping->associate('a' + i, '?');	
	}
}

TranslatorImpl::~TranslatorImpl()
{
	//delete both allocated currMappings
	delete m_currCtoPMapping;		
	delete m_currPtoCMapping;		

	//for each MyHash pointer in both of my lists, delete the MyHash it points to, then erase its place in the list
	std::list<MyHash<char, char>*>::iterator p1 = m_myStackOfCtoPMappings.begin();	
	while (p1 != m_myStackOfCtoPMappings.end())										
	{																				
		delete *p1;																	
		p1 = m_myStackOfCtoPMappings.erase(p1);										
	}																				
																					
	std::list<MyHash<char, char>*>::iterator p2 = m_myStackOfPtoCMappings.begin();	
	while (p2 != m_myStackOfPtoCMappings.end())										
	{																				
		delete *p2;																	
		p2 = m_myStackOfPtoCMappings.erase(p2);										
	}																				
}

//O(N + L)
bool TranslatorImpl::pushMapping(std::string ciphertext, std::string plaintext)
{
	//check params have the same length, return false if they don't
	if (ciphertext.size() != plaintext.size())	
		return false;							

	//for each letter in both params
	for (unsigned int i = 0; i < plaintext.size(); i++)	
	{
		//convert both letters to lower case
		ciphertext[i] = tolower(ciphertext[i]);	
		plaintext[i] = tolower(plaintext[i]);	

		//if either has a non-letter, or they aren't consistent, return false
		if (!isalpha(ciphertext[i]) || !isalpha(plaintext[i]) || isInconsistent(ciphertext[i], plaintext[i]))	
			return false;																						
	}	//at this point the letters should be added to mappings bc theyre consistent

	//copy the current c->p mapping and add it to my c->p stack (implemented as a list)
	//copy the current p->c mapping and add it to my p->c stack (implemented as a list)
	MyHash<char, char>* CtoPMappingToSave = new MyHash<char, char>;					
	MyHash<char, char>* PtoCMappingToSave = new MyHash<char, char>;					
	for (int i = 0; i < 26; i++)													
	{																				
		CtoPMappingToSave->associate('a' + i, *(m_currCtoPMapping->find('a' + i)));	
		PtoCMappingToSave->associate('a' + i, *(m_currPtoCMapping->find('a' + i)));	
	}																				
	m_myStackOfCtoPMappings.push_front(CtoPMappingToSave);							
	m_myStackOfPtoCMappings.push_front(PtoCMappingToSave);							

	//for each letter in both params, 
	for (unsigned int i = 0; i < plaintext.size(); i++)				
	{														
		//associate the cipherletter to the plainletter in current c->p mapping
		m_currCtoPMapping->associate(ciphertext[i], plaintext[i]);	
		//associate the plainLetter to the cipherLetter in current p->c mapping
		m_currPtoCMapping->associate(plaintext[i], ciphertext[i]);	
	}																

	//update this to keep track of successful pushes
	m_timesPushed++;	
	//then return true because successful
	return true;		
}

//O(1)
bool TranslatorImpl::popMapping()
{
	//if there's noting to pop return false
	if (m_timesPopped == m_timesPushed)	
		return false;					

	//delete the two currently dynamically-allcated mappings
	delete m_currCtoPMapping;		
	delete m_currPtoCMapping;		

	//set both current mappings to the first mappings stored in their respective stacks (most recently pushed)
	m_currCtoPMapping = *m_myStackOfCtoPMappings.begin();	
	m_currPtoCMapping = *m_myStackOfPtoCMappings.begin();	

	//then pop both of their stacks
	m_myStackOfCtoPMappings.pop_front();	
	m_myStackOfPtoCMappings.pop_front();	

	//update this to keep track
	m_timesPopped++;	
	//return true because successful
	return true;		
}

//O(N), N is lenth of param.
std::string TranslatorImpl::getTranslation(const std::string& ciphertext) const	
{
	//string to build and return
	std::string translationToReturn;	
	//for each letter in ciphertext
	for (unsigned int i = 0; i < ciphertext.size(); i++)	
	{
		//if the letter is uppercase add the uppercase version of the letter it maps to
		if (isupper(ciphertext[i]))																
			translationToReturn += toupper(*m_currCtoPMapping->find(tolower(ciphertext[i])));	
		//if the letter is lowercase add the lowercase version of the letter it maps to
		else if (islower(ciphertext[i]))														
			translationToReturn += *m_currCtoPMapping->find(tolower(ciphertext[i]));
		//otherwise, just add the character
		else																					
			translationToReturn += ciphertext[i];												
	}
	//return the string after following the map of each cipher letter to plaintext letter
	return translationToReturn;	
}

bool TranslatorImpl::isInconsistent(char cipherLetter, char plainLetter) const	//has O(1)
{
	const char* vp;

	//if cipherLetter already has a mapping and its different from plainLetter then the pairing is inconsistent
	vp = m_currCtoPMapping->find(cipherLetter);	 
	if (*vp != '?' && *vp != plainLetter)		
		return true;							

	//if plainLetter already has a mapping and its different from cipherLetter then the pairing is inconsistent 
	vp = m_currPtoCMapping->find(plainLetter);	
	if (*vp != '?' && *vp != cipherLetter)		
		return true;							

	//if neither of the above cases are true, the pairing is consistent with the current mapping
	return false;	
}

////////////////////////////////////////////////////////////////////////////////
//******************** Translator functions ************************************
////////////////////////////////////////////////////////////////////////////////

// These functions simply delegate to TranslatorImpl's functions.
// You probably don't want to change any of this code.

Translator::Translator()
{
	m_impl = new TranslatorImpl;
}

Translator::~Translator()
{
	delete m_impl;
}

bool Translator::pushMapping(std::string ciphertext, std::string plaintext)
{
	return m_impl->pushMapping(ciphertext, plaintext);
}

bool Translator::popMapping()
{
	return m_impl->popMapping();
}

std::string Translator::getTranslation(const std::string& ciphertext) const
{
	return m_impl->getTranslation(ciphertext);
}

