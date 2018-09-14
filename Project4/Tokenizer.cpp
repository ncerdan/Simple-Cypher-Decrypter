#include "provided.h"

class TokenizerImpl
{
public:
	TokenizerImpl(std::string separators);
	std::vector<std::string> tokenize(const std::string& s) const;
private:
	std::string m_separators;
	//returns true if c is a separator
	bool isSeparator(const char c) const;	
};

//O(P), P = number of separators
TokenizerImpl::TokenizerImpl(std::string separators)
	: m_separators(separators)
{}

//O(S*P), S = s length, P = number seperators
std::vector<std::string> TokenizerImpl::tokenize(const std::string& s) const
{
	std::vector<std::string> toReturn;
	std::string temp = "";
	//for each character in s
	for (int i = 0; i < s.size(); i++)	
	{
		//if it is not a separator, add it to temp
		if (!isSeparator(s[i]))			
			temp += s[i];
		//if it is a seperator AND temp isn't empty, add temp to vector and restart building word
		else if (temp != "")			
		{								
			toReturn.push_back(temp);	
			temp = "";					
		}								
	}
	
	//if it ended without a punctuation and temp is a word, add it to the vector
	if (temp != "")					
		toReturn.push_back(temp);

	//return the vector of only words a no separators
	return toReturn;	
}

//O(P), P = number of seperators (not assigned, my own helper)
bool TokenizerImpl::isSeparator(const char c) const
{
	//for each char in vector of separators
	for (int i = 0; i < m_separators.size(); i++)	
	{
		//if its c return true
		if (m_separators[i] == c)	
			return true;
	}
	//if none are c, return false
	return false;	
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
