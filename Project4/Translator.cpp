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

	std::list<MyHash<char, char>*> m_myStackOfCtoPMappings;		//implementing one stack for each mapping as lists by only using push_front() and pop_front()
	std::list<MyHash<char, char>*> m_myStackOfPtoCMappings;		//	

	int m_timesPushed;	//to ensure I don't try to pop when it's empty
	int m_timesPopped;	//
};

TranslatorImpl::TranslatorImpl()
	: m_timesPushed(0), m_timesPopped(0)
{
	m_currCtoPMapping = new MyHash<char, char>;		//dynamically allocate both hashtables
	m_currPtoCMapping = new MyHash<char, char>;		//

	for (int i = 0; i < 26; i++)					//sets both mappings to a mapping from each letter a-z to '?'
	{												//
		m_currCtoPMapping->associate('a' + i, '?');	//
		m_currPtoCMapping->associate('a' + i, '?');	//
	}
}

TranslatorImpl::~TranslatorImpl()
{
	delete m_currCtoPMapping;		//delete both allocated currMappings
	delete m_currPtoCMapping;		//

	std::list<MyHash<char, char>*>::iterator p1 = m_myStackOfCtoPMappings.begin();	//for each MyHash pointer in both of my lists, delete the MyHash it points to, then erase its place in the list
	while (p1 != m_myStackOfCtoPMappings.end())										//
	{																				//
		delete *p1;																	//
		p1 = m_myStackOfCtoPMappings.erase(p1);										//
	}																				//
																					//
	std::list<MyHash<char, char>*>::iterator p2 = m_myStackOfPtoCMappings.begin();	//
	while (p2 != m_myStackOfPtoCMappings.end())										//
	{																				//
		delete *p2;																	//
		p2 = m_myStackOfPtoCMappings.erase(p2);										//
	}																				//
}

bool TranslatorImpl::pushMapping(std::string ciphertext, std::string plaintext)	//FINISHED. has correct O(N+L)
{
	if (ciphertext.size() != plaintext.size())	//check params have the same length, return false if they don't
		return false;							//

	for (unsigned int i = 0; i < plaintext.size(); i++)	//for each letter in both params
	{
		ciphertext[i] = tolower(ciphertext[i]);	//convert both letters to lower case
		plaintext[i] = tolower(plaintext[i]);	//

		if (!isalpha(ciphertext[i]) || !isalpha(plaintext[i]) || isInconsistent(ciphertext[i], plaintext[i]))	//if either has a non-letter, or they aren't consistent, return false
			return false;																						//
	}	//at this point the letters should be added to mappings bc theyre consistent

	MyHash<char, char>* CtoPMappingToSave = new MyHash<char, char>;					//copy the current c->p mapping and add it to my c->p stack (implemented as a list)
	MyHash<char, char>* PtoCMappingToSave = new MyHash<char, char>;					//copy the current p->c mapping and add it to my p->c stack (implemented as a list)
	for (int i = 0; i < 26; i++)													//
	{																				//
		CtoPMappingToSave->associate('a' + i, *(m_currCtoPMapping->find('a' + i)));	//
		PtoCMappingToSave->associate('a' + i, *(m_currPtoCMapping->find('a' + i)));	//
	}																				//
	m_myStackOfCtoPMappings.push_front(CtoPMappingToSave);							//
	m_myStackOfPtoCMappings.push_front(PtoCMappingToSave);							//

	for (unsigned int i = 0; i < plaintext.size(); i++)				//then for each letter in both params, 
	{																//
		m_currCtoPMapping->associate(ciphertext[i], plaintext[i]);	//associate the cipherletter to the plainletter in current c->p mapping
		m_currPtoCMapping->associate(plaintext[i], ciphertext[i]);	//associate the plainLetter to the cipherLetter in current p->c mapping
	}																//


	m_timesPushed++;	//update this to keep track of successful pushes
	return true;		//then return true because successful
}

bool TranslatorImpl::popMapping()	//FINISHED. has correct O(1)
{
	if (m_timesPopped == m_timesPushed)	//if there's noting to pop return false
		return false;					//

	delete m_currCtoPMapping;		//delete the two currently dynamically-allcated mappings
	delete m_currPtoCMapping;		//

	m_currCtoPMapping = *m_myStackOfCtoPMappings.begin();	//set both current mappings to the first mappings stored in their respective stacks (most recently pushed)
	m_currPtoCMapping = *m_myStackOfPtoCMappings.begin();	//

	m_myStackOfCtoPMappings.pop_front();	//then pop both of their stacks
	m_myStackOfPtoCMappings.pop_front();	//

	m_timesPopped++;	//update this to keep track
	return true;		//return true because successful
}

std::string TranslatorImpl::getTranslation(const std::string& ciphertext) const	//FINISHED. has O(N), N is lenth of param. no spec requirement of bigO
{
	std::string translationToReturn;	//string to build and return
	for (unsigned int i = 0; i < ciphertext.size(); i++)	//for each letter in ciphertext
	{
		if (isupper(ciphertext[i]))																//if the letter is uppercase add the uppercase version of the letter it maps to
			translationToReturn += toupper(*m_currCtoPMapping->find(tolower(ciphertext[i])));	//
		else if (islower(ciphertext[i]))														//if the letter is lowercase add the lowercase version of the letter it maps to
			translationToReturn += *m_currCtoPMapping->find(tolower(ciphertext[i]));			//
		else																					//otherwise, just add the character
			translationToReturn += ciphertext[i];												//
	}
	return translationToReturn;	//return the string after following the map of each cipher letter to plaintext letter
}

bool TranslatorImpl::isInconsistent(char cipherLetter, char plainLetter) const	//has O(1)
{
	const char* vp;		//declare object

	vp = m_currCtoPMapping->find(cipherLetter);	//if cipherLetter already has a mapping and its different from plainLetter then the pairing is inconsistent 
	if (*vp != '?' && *vp != plainLetter)		//
		return true;							//

	vp = m_currPtoCMapping->find(plainLetter);	//if plainLetter already has a mapping and its different from cipherLetter then the pairing is inconsistent 
	if (*vp != '?' && *vp != cipherLetter)		//
		return true;							//

	return false;	//if neither of the above cases are true, the pairing is consistent with the current mapping
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

