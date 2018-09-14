#include "provided.h"

class TokenizerImpl
{
public:
	TokenizerImpl(std::string separators);
	std::vector<std::string> tokenize(const std::string& s) const;
private:
	std::string m_separators;
	bool isSeparator(const char c) const;	//returns true if c is a separator
};

TokenizerImpl::TokenizerImpl(std::string separators)	//FINSIHED. has correct O(P), P = number of separators
	: m_separators(separators)
{}

std::vector<std::string> TokenizerImpl::tokenize(const std::string& s) const	//FINISED. has correct O(S*P), S = s length, P = number seperators
{
	std::vector<std::string> toReturn;
	std::string temp = "";
	for (int i = 0; i < s.size(); i++)	//for each character in s
	{
		if (!isSeparator(s[i]))			//if it is not a separator, add it to temp
			temp += s[i];
		else if (temp != "")			//if it is a seperator AND temp isn't empty, add temp to vector and restart building word
		{								//
			toReturn.push_back(temp);	//
			temp = "";					//
		}								//
	}
	
	if (temp != "")					//if it ended without a punctuation and temp is a word, add it to the vector
		toReturn.push_back(temp);	//

	return toReturn;	//return the vector of only words a no separators
}

bool TokenizerImpl::isSeparator(const char c) const		//FINISHED. has O(P), P = number of seperators (not assigned, my own helper)
{
	for (int i = 0; i < m_separators.size(); i++)	//for each char in vector of separators
	{
		if (m_separators[i] == c)	//if its c return true
			return true;
	}
	return false;	//if none are c, return false
}

///////////////////////////////////////////////////////////////////////////////
//******************** Tokenizer functions ************************************
///////////////////////////////////////////////////////////////////////////////

// These functions simply delegate to TokenizerImpl's functions.
// You probably don't want to change any of this code.

Tokenizer::Tokenizer(std::string separators)
{
	m_impl = new TokenizerImpl(separators);
}

Tokenizer::~Tokenizer()
{
	delete m_impl;
}

std::vector<std::string> Tokenizer::tokenize(const std::string& s) const
{
	return m_impl->tokenize(s);
}
