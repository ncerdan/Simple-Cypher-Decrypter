#include "provided.h"
#include <algorithm>	
#include <list>			

class DecrypterImpl
{
public:
	DecrypterImpl();
	bool load(std::string filename);
	std::vector<std::string> crack(const std::string& ciphertext);
private:
	const std::string SEPARATORS = "0123456789 ,;:.!()[]{}-\"#$%^&";
	WordList m_dictionary;
	Translator m_translator;
	Tokenizer m_tokenizer;
	std::list<std::string> m_stackOfWordsUsed;
	//use as stack, so only push_front(), and pop_front()

	//returns the string out of v with the most unkown letters after translation
	std::string getWordWMostLettersWNoTranslation(std::vector<std::string> v);
	//returns vector holding all words that are fully translated after translation
	std::vector<std::string> getFullyTranslatedWords(std::string ciphertext);		
	//returns true if all characters are known, false if otherwise
	bool isFullyTranslated(std::string plainTextTranslation);						
};

//creates tokenizer. will allow other members to default construct
DecrypterImpl::DecrypterImpl()	
	: m_tokenizer(SEPARATORS)
{}

//O(W), W = number of words in file
bool DecrypterImpl::load(std::string filename)	
{
	return m_dictionary.loadWordList(filename);
}

std::vector<std::string> DecrypterImpl::crack(const std::string& ciphertext)
{
	//vector to build and return
	std::vector<std::string> toReturn;	
	//creates a vector which holds all cipher words in the text
	std::vector<std::string> vectorOfAllCipherWords = m_tokenizer.tokenize(ciphertext);	

	//gets word with the most unknown letters after translation that hasn't already been chosen
	std::string w = getWordWMostLettersWNoTranslation(vectorOfAllCipherWords);	
	//gets translation of this word
	std::string translationOfW = m_translator.getTranslation(w);				

	//gets C, a collection of possible words that w could be given our current translation mapping
	std::vector<std::string> C = m_dictionary.findCandidates(w, translationOfW);	

	//if there are no possible words with current translation, take that word off the stack, throw out mapping and return recursive call empty
	if (C.empty())							
	{										
		m_stackOfWordsUsed.pop_front();		
		m_translator.popMapping();			
		return std::vector<std::string>();		
	}										

	//for each possible word p in C
	for (int i = 0; i < C.size(); i++)		
	{
		//if the mapping from w to p would be incompatible with the current mapping go to next p
		if (!m_translator.pushMapping(w, C[i]))	 
			continue;							
											
		//use new translation to translate entire message
		std::string translationOfEntireCipherText = m_translator.getTranslation(ciphertext);	
		//get all fully translated words using new/current translation mapping
		std::vector<std::string> allFullyTranslatedWords = getFullyTranslatedWords(ciphertext);	

		bool wordIsFound;
		//for each fully-translated word
		for (int j = 0; j < allFullyTranslatedWords.size(); j++)	
		{
			wordIsFound = true;
			//if any one fully-translated word is not found in the dictionary
			if (!m_dictionary.contains(allFullyTranslatedWords[j]))	
			{
				//get rid of the current, incorrect mapping, set this to false to go to next p, and end loop through translated words
				m_translator.popMapping();	
				wordIsFound = false;		
				break;						
			}
		}
		//if any fully-translated word after pushing new translation isn't found, must go to next p immediately
		if (!wordIsFound)	
			continue;		
	
		//at this point all fully-translated words are valid english words
		//if the message has not been fully translated
		if (!isFullyTranslated(m_translator.getTranslation(ciphertext)))	
		{
			//recursively call step 2, passing in our temporary mapping as the new input mapping
			std::vector<std::string> recursiveResult = crack(ciphertext);
			//add recursive result to vector to return
			toReturn.insert(toReturn.end(), recursiveResult.begin(), recursiveResult.end());		
		}
		//if the message is fully translated
		else		
		{
			//record this as a valid solution for eventual output to the user
			toReturn.push_back(m_translator.getTranslation(ciphertext));	
			//discard this mapping since it got us the result
			m_translator.popMapping();	
			//and go to next p
			continue;					
		}
	}

	//Having tried all the words in our collection C, throw away the current mapping table pop from stack and return to the previous recursive call
	m_translator.popMapping();
	m_stackOfWordsUsed.pop_front();
	//alphabetize vector, and return it
	std::sort(toReturn.begin(), toReturn.end());	
	return toReturn;
}

std::string DecrypterImpl::getWordWMostLettersWNoTranslation(std::vector<std::string> v)
{
	int mostUnknowns = 0;
	int indexOfWordWMostUnknowns = 0;
	//for each word in the vector v
	for (int i = 0; i < v.size(); i++)	
	{
		std::string currWord = v[i];
		std::string currWordsTranslation = m_translator.getTranslation(currWord);
		int numUnknowns = 0;
		//for each letter of this word's translation
		for (int j = 0; j < currWordsTranslation.size(); j++)	
		{
			//if it is unknown, update var
			if (currWordsTranslation[j] == '?')	
				numUnknowns++;					
		}
		//if the word hasn't already been used (stored in my list) and it has the most unknowns so far, save it
		if (find(m_stackOfWordsUsed.begin(), m_stackOfWordsUsed.end(), currWord) == m_stackOfWordsUsed.end()
			&& numUnknowns > mostUnknowns)				
		{												
			mostUnknowns = numUnknowns;					
			indexOfWordWMostUnknowns = i;				
		}												
	}

	//save the word that I'm going to return to my list, then return it
	m_stackOfWordsUsed.push_front(v[indexOfWordWMostUnknowns]);	
	return v[indexOfWordWMostUnknowns];							
}

std::vector<std::string> DecrypterImpl::getFullyTranslatedWords(std::string ciphertext)
{
	std::string translation = m_translator.getTranslation(ciphertext);
	std::vector<std::string> allTranslatedWords = m_tokenizer.tokenize(translation);
	std::vector<std::string> fullyTranslatedWords;

	//for each word in allTranslatedWords
	for (int i = 0; i < allTranslatedWords.size(); i++)		
	{
		std::string currWord = allTranslatedWords[i];
		//if every letter in the word is known add it to the vector to return
		if (isFullyTranslated(currWord))				
			fullyTranslatedWords.push_back(currWord);	
	}
	//after every word has been looked at, return this vector
	return fullyTranslatedWords;	
}

bool DecrypterImpl::isFullyTranslated(std::string translation)
{
	//for each letter in the translation
	for (int j = 0; j < translation.size(); j++)	
	{
		//if a letter is unknown in the translation, return false
		if (translation[j] == '?')			
			return false;
	}
	//if all letters are known return true;
	return true;	
}

///////////////////////////////////////////////////////////////////////////////
//******************** Decrypter functions ************************************
///////////////////////////////////////////////////////////////////////////////

// These functions simply delegate to DecrypterImpl's functions.
// You probably don't want to change any of this code.

Decrypter::Decrypter()
{
	m_impl = new DecrypterImpl;
}

Decrypter::~Decrypter()
{
	delete m_impl;
}

bool Decrypter::load(std::string filename)
{
	return m_impl->load(filename);
}

std::vector<std::string> Decrypter::crack(const std::string& ciphertext)
{
	return m_impl->crack(ciphertext);
}
