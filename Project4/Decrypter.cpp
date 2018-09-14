#include "provided.h"
#include <algorithm>	//for sort(), find()
#include <list>			//for member

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

	std::string getWordWMostLettersWNoTranslation(std::vector<std::string> v);		//returns the string out of v with the most unkown letters after translation
	std::vector<std::string> getFullyTranslatedWords(std::string ciphertext);		//returns vector holding all words that are fully translated after translation
	bool isFullyTranslated(std::string plainTextTranslation);						//returns true if all characters are known, false if otherwise
};

DecrypterImpl::DecrypterImpl()	//creates tokenizer. will allow other members to default construct
	: m_tokenizer(SEPARATORS)
{}

bool DecrypterImpl::load(std::string filename)	//FINISHED. has corret O(W), W = number of words in file
{
	return m_dictionary.loadWordList(filename);
}

std::vector<std::string> DecrypterImpl::crack(const std::string& ciphertext)
{
	std::vector<std::string> toReturn;	//vector to build and return
	std::vector<std::string> vectorOfAllCipherWords = m_tokenizer.tokenize(ciphertext);	//creates a vector which holds all cipher words in the text

	std::string w = getWordWMostLettersWNoTranslation(vectorOfAllCipherWords);	//gets word with the most unknown letters after translation that hasn't already been chosen
	std::string translationOfW = m_translator.getTranslation(w);				//gets translation of this word

	std::vector<std::string> C = m_dictionary.findCandidates(w, translationOfW);	//gets C, a collection of possible words that w could be given our current translation mapping

	if (C.empty())							//if there are no possible words with current translation, take that word off the stack, throw out mapping and return recursive call empty
	{										//
		m_stackOfWordsUsed.pop_front();		//
		m_translator.popMapping();			//
		return std::vector<std::string>();	//	
	}										//

	for (int i = 0; i < C.size(); i++)		//for each possible word p in C
	{
		if (!m_translator.pushMapping(w, C[i]))	//if the mapping from w to p would be incompatible with the current mapping go to next p 
			continue;							//(no need to pop anything bc it wont push this newest mapping)
												//at this point it pushed the mapping guided by w->p
		std::string translationOfEntireCipherText = m_translator.getTranslation(ciphertext);	//use new translation to translate entire message
		std::vector<std::string> allFullyTranslatedWords = getFullyTranslatedWords(ciphertext);	//get all fully translated words using new/current translation mapping

		bool wordIsFound;
		for (int j = 0; j < allFullyTranslatedWords.size(); j++)	//for each fully-translated word
		{
			wordIsFound = true;
			if (!m_dictionary.contains(allFullyTranslatedWords[j]))	//if any one fully-translated word is not found in the dictionary
			{
				m_translator.popMapping();	//get rid of the current, incorrect mapping, set this to false to go to next p, and end loop through translated words
				wordIsFound = false;		//
				break;						//
			}
		}
		if (!wordIsFound)	//if any fully-translated word after pushing new translation isn't found, must go to next p immediately
			continue;		//
							//at this point all fully-translated words are valid english words
		if (!isFullyTranslated(m_translator.getTranslation(ciphertext)))	//if the message has not been fully translated
		{
			std::vector<std::string> recursiveResult = crack(ciphertext);	//recursively call step 2, passing in our temporary mapping as the new input mapping (idk if this is right)
			toReturn.insert(toReturn.end(), recursiveResult.begin(), recursiveResult.end());	//add recursive result to vector to return	
		}
		else		//if the message is fully translated
		{
			toReturn.push_back(m_translator.getTranslation(ciphertext));	// record this as a valid solution for eventual output to the user
			m_translator.popMapping();	//discard this mapping since it got us the result
			continue;					//and go to next p
		}
	}

	//Having tried all the words in our collection C, throw away the current mapping table pop from stack and return to the previous recursive call
	m_translator.popMapping();
	m_stackOfWordsUsed.pop_front();
	std::sort(toReturn.begin(), toReturn.end());	//alphabetize vector, and return it
	return toReturn;								//
}

std::string DecrypterImpl::getWordWMostLettersWNoTranslation(std::vector<std::string> v)
{
	int mostUnknowns = 0;
	int indexOfWordWMostUnknowns = 0;
	for (int i = 0; i < v.size(); i++)	//for each word in the vector v
	{
		std::string currWord = v[i];
		std::string currWordsTranslation = m_translator.getTranslation(currWord);
		int numUnknowns = 0;
		for (int j = 0; j < currWordsTranslation.size(); j++)	//for each letter of this word's translation
		{
			if (currWordsTranslation[j] == '?')	//if it is unknown, update var
				numUnknowns++;					//
		}
		if (find(m_stackOfWordsUsed.begin(), m_stackOfWordsUsed.end(), currWord) == m_stackOfWordsUsed.end()
			&& numUnknowns > mostUnknowns)				//if the word hasn't already been used (stored in my list) and it has the most unknowns so far, save it
		{												//
			mostUnknowns = numUnknowns;					//
			indexOfWordWMostUnknowns = i;				//
		}												//
	}

	m_stackOfWordsUsed.push_front(v[indexOfWordWMostUnknowns]);	//save the word that I'm going to return to my list, then return it
	return v[indexOfWordWMostUnknowns];							//
}

std::vector<std::string> DecrypterImpl::getFullyTranslatedWords(std::string ciphertext)
{
	std::string translation = m_translator.getTranslation(ciphertext);
	std::vector<std::string> allTranslatedWords = m_tokenizer.tokenize(translation);
	std::vector<std::string> fullyTranslatedWords;

	for (int i = 0; i < allTranslatedWords.size(); i++)		//for each word in allTranslatedWords
	{
		std::string currWord = allTranslatedWords[i];
		if (isFullyTranslated(currWord))				//if every letter in the word is known add it to the vector to return
			fullyTranslatedWords.push_back(currWord);	//
	}
	return fullyTranslatedWords;	//after every word has been looked at, return this vector
}

bool DecrypterImpl::isFullyTranslated(std::string translation)	//need to test
{
	for (int j = 0; j < translation.size(); j++)	//for each letter in the translation
	{
		if (translation[j] == '?')			//if a letter is unknown in the translation, return false
			return false;
	}
	return true;	//if all letters are known return true;
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
