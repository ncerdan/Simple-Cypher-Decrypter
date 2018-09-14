#include "provided.h"
#include "MyHash.h"
#include <fstream>
#include <iostream>

///////////// Given Helper Functions //////////////
//hash for string
unsigned int hash(const std::string& s)	
{
	return std::hash<std::string>()(s);
}
//hash for int
unsigned int hash(const int& u)			
{
	return std::hash<int>()(u);
}
//hash for char
unsigned int hash(const	char& c)		
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

//does not need to do anything, bc it will allow the MyHash obj to default construct
WordListImpl::WordListImpl()
{}

//O(W) where W is the number of words in file
bool WordListImpl::loadWordList(std::string dictFilename)
{
	//reset both tables
	m_wordTable.reset();	
	m_hasAllWords.reset();

	std::ifstream infile(dictFilename);
	//if it didn't find the file, return false
	if (!infile)		
		return false;

	//get every line (which has one word each), and for each word...
	std::string s;					
	while (getline(infile, s))		
	{
		//if the word has any character that is not a letter or apostrophe, go to next line in the text file, otherwise make it lowercase 
		bool isGood = true;							
		for (unsigned int i = 0; i < s.size(); i++)	
		{											
			if (!isalpha(s[i]) && s[i] != '\'')		
			{										
				isGood = false;							
				break;								
			}										
			s[i] = tolower(s[i]);					
		}											
		if (!isGood)								
			continue;								

		//if the letter pattern of the word hasnt been seen before
		std::vector<std::string>* vp = m_wordTable.find(getLetterPattern(s));
		if (vp == nullptr)	
		{
			//create a new vector with the word and associate it with the letter pattern
			std::vector<std::string> newVector;						
			newVector.push_back(s);									
			m_wordTable.associate(getLetterPattern(s), newVector);	
		}
		//if the pattern has been seen
		else				
			//add the word to the vector associated with the pattern
			vp->push_back(s);	

		//then add the word to this vector regardless of whether it has a new or old pattern
		m_hasAllWords.associate(s, true);	
	}

	//return true because it was successful
	return true;	
}

//O(1)
bool WordListImpl::contains(std::string word) const		
{
	//convert word to lowercase to make this function case-insensitive
	for (unsigned int i = 0; i < word.size(); i++)	
		word[i] = tolower(word[i]);					
	
	//try to find the word. //if it didn't, return false. if it did, return true
	const bool* vp = m_hasAllWords.find(word);	
	if (vp == nullptr)	
		return false;	
	return true;		
}

//O(Q), Q = numWords w right pattern
std::vector<std::string> WordListImpl::findCandidates(std::string cipherWord, std::string currTranslation) const
{
	//create a vector to return, we will build up then return this at the end
	std::vector<std::string> vectorToFillAndReturn;		

	//check the two params have the same length, if not, return empty
	if (cipherWord.size() != currTranslation.size())	
		return std::vector<std::string>();				

	bool isGood = true;
	//for each letter in both params
	for (unsigned int l = 0; l < cipherWord.size(); l++)
	{													
		//change the letters to lowercase because it's case insensitive
		cipherWord[l] = tolower(cipherWord[l]);				
		currTranslation[l] = tolower(currTranslation[l]);	

		//check that cipherWord has the right characters. if not, set var to bad, and break loop
		if (!isalpha(cipherWord[l]) && cipherWord[l] != '\'')	 
		{														
			isGood = false;										
			break;												
		}
		//check that currTranslation has the right characters. if not, set var to bad, and break loop
		if (!isalpha(currTranslation[l]) && currTranslation[l] != '\'' && currTranslation[l] != '?')	
		{																								
			isGood = false;																				
			break;																						
		}																								
	}
	//if there was any bad character in the params, return empty
	if (!isGood)							
		return std::vector<std::string>();	

	//if no words in the dictionary share cipherWords pattern, return empty vector
	const std::vector<std::string>* vp = m_wordTable.find(getLetterPattern(cipherWord));	
	if (vp == nullptr)																		
		return std::vector<std::string>();													

	//otherwise, vp points to the vector of words with the right letter pattern
	//for each word in the vector associated with the right pattern
	for (unsigned int i = 0; i < (*vp).size(); i++)	
	{
		bool shouldAddWord = true;
		std::string currWord = (*vp)[i];
		//for each letter in this word
		for (unsigned int j = 0; j < currWord.size(); j++)	
		{
			//if currTranslation[j] is a letter: return empty if currWord[j] isn't a letter, else go to next word if currWord[j] != currTranslation[i]
			if (isalpha(currTranslation[j]))				 
			{												
				if (!isalpha(cipherWord[j]))						
					return std::vector<std::string>();		
				if (currTranslation[j] != currWord[j])				
				{											
					shouldAddWord = false;					
					break;									
				}
			}
			//if currTranslation[j] is a '?': return empty if cipherWord[j] isn't a letter
			else if (currTranslation[j] == '?')			
			{											
				if (!isalpha(cipherWord[j]))					
					return std::vector<std::string>();	
			}
			//if currTranslation[j] is a ': return empty if cipherWord[j] != ', else go to next word if currWord[j] != '
			else if (currTranslation[j] == '\'')		
			{											
				if (cipherWord[j] != '\'')						
					return std::vector<std::string>();	
				if (currWord[j] != '\'')						
				{										
					shouldAddWord = false;				
					break;								
				}
			}
		}
		//if the word didn't have any issues, add it to the vector to return at the end
		if (shouldAddWord)								
			vectorToFillAndReturn.push_back(currWord);	
	}
	//after getting through all words, return vector of potential words
	return vectorToFillAndReturn;	
}


//O(L), L = length of word
std::string WordListImpl::getLetterPattern(std::string word) const 
{
	//change the input to all lowercase so its cap-insensitive
	for (unsigned int i = 0; i < word.size(); i++)	
		word[i] = tolower(word[i]);					

	//hash table to keep track of characters already seen and their pattern pairing
	MyHash<char, char> charsSeen;		
	//string to build to return at end
	std::string letterPatternToReturn;	

	//creates string of CAP LETTERS and an int to keep track of next letter to use to code pattern
	std::string CAPLetters = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";	
	int nextCAPLetterToUseIndex = 0;						

	//for each letter in word
	for (unsigned int i = 0; i < word.size(); i++)	
	{
		//if the letter hasn't been seen yet
		if (charsSeen.find(word[i]) == nullptr)	
		{
			//add it's pattern letter to the pattern string, save that we've seen this letter, increment int so next new letter gets next CAP letter
			letterPatternToReturn += CAPLetters[nextCAPLetterToUseIndex];		
			charsSeen.associate(word[i], CAPLetters[nextCAPLetterToUseIndex]);	
			nextCAPLetterToUseIndex++;											
		}
		//if the letter has been seen already, which means its in charsSeen	
		else							
			//add the letter's pattern pairing to the pattern string
			letterPatternToReturn += *(charsSeen.find(word[i]));				
	}
	//after building the string, return it
	return letterPatternToReturn;	
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