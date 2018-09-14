#include "provided.h"
#include "MyHash.h"
#include <fstream>					//for file reading
#include <iostream>					//

///////////// Given Helper Functions //////////////
unsigned int hash(const std::string& s)	//hash for string
{
	return std::hash<std::string>()(s);
}
unsigned int hash(const int& u)			//hash for int
{
	return std::hash<int>()(u);
}

unsigned int hash(const	char& c)		//hash for char
{
	return std::hash<char>()(c);
}

class WordListImpl
{
public:
	WordListImpl();
	bool loadWordList(std::string dictFilename);
	bool contains(std::string word)	const;
	std::vector<std::string> findCandidates(std::string cipherWord, std::string currTranslation) const;
private:
	MyHash<std::string, std::vector<std::string>> m_wordTable;
	// the KeyType is std::string and will represent the letter pattern w all CAP letters (turtle = ABCADE)
	// the ValueType is a vector of strings that holds all the words in the list that have that pattern

	MyHash<std::string, bool> m_hasAllWords;
	// the KeyType is std::string and represents each word found in the file
	// the ValueType is a bool bc it is cheapest and doesn't matter
	
	std::string getLetterPattern(std::string) const;
};


WordListImpl::WordListImpl()	//does not need to do anything, bc it will allow the MyHash obj to default construct
{}

bool WordListImpl::loadWordList(std::string dictFilename)	//FINISHED. has correct O(W) where W is the number of words in file
{
	m_wordTable.reset();	//reset both tables
	m_hasAllWords.reset();	//

	std::ifstream infile(dictFilename);
	if (!infile)		//if it didn't find the file, return false
		return false;	//

	std::string s;					//get every line (which has one word each), and for each word
	while (getline(infile, s))		//
	{
		bool isGood = true;							//if the word has any character that is not a letter or apostrophe, go to next line in the text file, otherwise make it lowercase 
		for (unsigned int i = 0; i < s.size(); i++)	//
		{											//
			if (!isalpha(s[i]) && s[i] != '\'')		//
			{										//
				isGood = false;						//	
				break;								//
			}										//
			s[i] = tolower(s[i]);					//
		}											//
		if (!isGood)								//
			continue;								//

		std::vector<std::string>* vp = m_wordTable.find(getLetterPattern(s));
		if (vp == nullptr)	//if the letter pattern of the word hasnt been seen before
		{
			std::vector<std::string> newVector;						//create a new vector with the word and associate it with the letter pattern
			newVector.push_back(s);									//
			m_wordTable.associate(getLetterPattern(s), newVector);	//
		}
		else				//if the pattern has been seen
			vp->push_back(s);	//add the word to the vector associated with the pattern

		m_hasAllWords.associate(s, true);	//then add the word to this vector regardless of whether it has a new or old pattern
	}

	return true;	//return true because it was successful
}

bool WordListImpl::contains(std::string word) const		//FINISHED. has correct O(1)
{
	for (unsigned int i = 0; i < word.size(); i++)	//convert word to lowercase to make this function case-insensitive
		word[i] = tolower(word[i]);					//
	
	const bool* vp = m_hasAllWords.find(word);	//try to find the word
	if (vp == nullptr)	//if it didn't, return false
		return false;	//
	return true;		//if it did, return true
}

std::vector<std::string> WordListImpl::findCandidates(std::string cipherWord, std::string currTranslation) const	//FINISHED. has correct O(Q), Q = numWords w right pattern
{
	std::vector<std::string> vectorToFillAndReturn;		//create a vectors to return, we will build up then return this at the end

	if (cipherWord.size() != currTranslation.size())	//check the two params have the same length, if not, return empty
		return std::vector<std::string>();				//

	bool isGood = true;
	for (unsigned int l = 0; l < cipherWord.size(); l++)//for each letter in both params
	{													//
		cipherWord[l] = tolower(cipherWord[l]);				//change the letters to lowercase because it's case insensitive
		currTranslation[l] = tolower(currTranslation[l]);	//

		if (!isalpha(cipherWord[l]) && cipherWord[l] != '\'')	//check that cipherWord has the right characters. if not, set var to bad, and break loop 
		{														//
			isGood = false;										//
			break;												//
		}
		if (!isalpha(currTranslation[l]) && currTranslation[l] != '\'' && currTranslation[l] != '?')	//check that currTranslation has the right characters. if not, set var to bad, and break loop
		{																								//
			isGood = false;																				//
			break;																						//
		}																								//
	}
	if (!isGood)							//if there was any bad character in the params, return empty
		return std::vector<std::string>();	//

	const std::vector<std::string>* vp = m_wordTable.find(getLetterPattern(cipherWord));	//if no words in the dictionary share cipherWords pattern, return empty vector
	if (vp == nullptr)																		//
		return std::vector<std::string>();													//

	//otherwise, vp points to the vector of words with the right letter pattern
	for (unsigned int i = 0; i < (*vp).size(); i++)	//for each word in the vector associated with the right pattern
	{
		bool shouldAddWord = true;	//to keep track
		std::string currWord = (*vp)[i];
		for (unsigned int j = 0; j < currWord.size(); j++)	//for each letter in this word
		{
			if (isalpha(currTranslation[j]))				//if currTranslation[j] is a letter: 
			{												//
				if (!isalpha(cipherWord[j]))				//		return empty if currWord[j] isn't a letter
					return std::vector<std::string>();		//
				if (currTranslation[j] != currWord[j])		//		go to next word if currWord[j] != currTranslation[i]
				{											//
					shouldAddWord = false;					//
					break;									//
				}
			}
			else if (currTranslation[j] == '?')			//if currTranslation[j] is a '?':
			{											//
				if (!isalpha(cipherWord[j]))			//		return empty if cipherWord[j] isn't a letter
					return std::vector<std::string>();	//
			}
			else if (currTranslation[j] == '\'')		//if currTranslation[j] is a ':
			{											//
				if (cipherWord[j] != '\'')				//		return empty if cipherWord[j] != '
					return std::vector<std::string>();	//
				if (currWord[j] != '\'')				//		go to next word if currWord[j] != '
				{										//
					shouldAddWord = false;				//
					break;								//
				}
			}
		}
		if (shouldAddWord)								//if the word didn't have any issues, add it to the vector to return at the end
			vectorToFillAndReturn.push_back(currWord);	//
	}
	return vectorToFillAndReturn;	//after getting through all words, return vector of potential words
}

std::string WordListImpl::getLetterPattern(std::string word) const	//FINISHED. O(L), L = length of word
{
	for (unsigned int i = 0; i < word.size(); i++)	//change the input to all lowercase so its cap-insensitive
		word[i] = tolower(word[i]);					//

	MyHash<char, char> charsSeen;		//hash table to keep track of characters already seen and their pattern pairing
	std::string letterPatternToReturn;	//string to build to return at end

	std::string CAPLetters = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";	//creates string of CAP LETTERS and an int to keep track of next letter to use to code pattern
	int nextCAPLetterToUseIndex = 0;						//

	for (unsigned int i = 0; i < word.size(); i++)	//for each letter in word
	{
		if (charsSeen.find(word[i]) == nullptr)	//if the letter hasn't been seen yet
		{
			letterPatternToReturn += CAPLetters[nextCAPLetterToUseIndex];		//add it's pattern letter to the pattern string
			charsSeen.associate(word[i], CAPLetters[nextCAPLetterToUseIndex]);	//save that we've seen this letter
			nextCAPLetterToUseIndex++;											//increment int so next new letter gets next CAP letter
		}
		else			//if the letter has been seen already, which means its in charsSeen					
			letterPatternToReturn += *(charsSeen.find(word[i]));		//add the letter's pattern pairing to the pattern string		
	}
	return letterPatternToReturn;	//after building the string, return it
}

//////////////////////////////////////////////////////////////////////////////
//******************** WordList functions ************************************
//////////////////////////////////////////////////////////////////////////////

// These functions simply delegate to WordListImpl's functions.
// You probably don't want to change any of this code.

WordList::WordList()
{
	m_impl = new WordListImpl;
}

WordList::~WordList()
{
	delete m_impl;
}

bool WordList::loadWordList(std::string filename)
{
	return m_impl->loadWordList(filename);
}

bool WordList::contains(std::string word) const
{
	return m_impl->contains(word);
}

std::vector<std::string> WordList::findCandidates(std::string cipherWord, std::string currTranslation) const
{
	return m_impl->findCandidates(cipherWord, currTranslation);
}