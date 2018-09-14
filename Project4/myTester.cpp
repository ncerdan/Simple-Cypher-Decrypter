#include "provided.h"
#include "MyHash.h"
#include <iostream>
#include <vector>
#include <cassert>

//Sanity Tests Passed:
//	MyHash
//	Tokenizer
//	WordList
//	Translator
//	Decrypter

int main()														//EACH ON OF THESE ARE SAVED ON G32
{
	// MyHash Tests ////////////////
	/* new array												1 WORKS ON BOTH COMPILERS
	MyHash<std::string, int> hm1(.01);
	hm1.associate("Even", 0);
	hm1.print();
	hm1.associate("Nick", 10000);
	hm1.print();
	hm1.associate("chad", 15);
	hm1.print();
	hm1.associate("Even", -1);
	hm1.print();
	hm1.associate("haley", 10);
	hm1.associate("a", 4);
	hm1.print();
	*/

	/*	tested basics and resets, no new array					2 WORKS ON BOTH COMPILERS
	MyHash<std::string, int> hm1(2);
	hm1.associate("Even", 0);
	hm1.print();
	hm1.associate("Nick", 10000);
	hm1.print();
	hm1.associate("chad", 15);
	hm1.print();
	hm1.associate("Even", -1);
	hm1.associate("haley", 10);
	hm1.reset();
	hm1.associate("haley", 69);
	hm1.print();
	*/
	/* tests different type of table							3 WORKS ON BOTH COMPILERS
	MyHash<int, char> hm2(.25);
	for (int i = 1; i < 27; i++)
		hm2.associate(i, 'a' - 1 + i);
	hm2.print();
	*/

	// Tokenizer Tests //////////////
	/* tests all functionality									4 WORKS ON BOTH COMPILERS
	TokenizerImpl t1(" ,.!?");
	t1.printSeps();
	std::vector<std::string> tokens = t1.tokenize("Hi, my name is nick and I like things. Hi!!!!       I love   this?? lastone");
	for (int i = 0; i < tokens.size(); i++)
		std::cout << tokens[i] << std::endl;
	*/

	// Wordlist Tests ///////////////
	/*	tests getLetterPattern()								5 WORKS ON BOTH COMPILERS
	WordListImpl w1;
	std::cout << ": " << w1.getLetterPattern("") << std::endl;
	std::cout << "asdf: " << w1.getLetterPattern("asdf") << std::endl;
	std::cout << "aAbBcC: " << w1.getLetterPattern("aAbBcC") << std::endl;
	std::cout << "Happy: " << w1.getLetterPattern("Happy") << std::endl;
	std::cout << "testsssssss: " << w1.getLetterPattern("testsssssss") << std::endl;
	std::cout << "qazwsxedcrfvtgbyhnujmikolpplokimjunhybgtvfrcdexswzaq: " << w1.getLetterPattern("qazwsxedcrfvtgbyhnujmikolpplokimjunhybgtvfrcdexswzaq") << std::endl;
	*/

	/*	tests loadWordList(), contains()						6 WORKS ON BOTH COMPILERS
	WordListImpl w2;
	w2.loadWordList("wordlist.txt");
	std::cout << "'car' is in list: " << w2.contains("car") << std::endl;
	std::cout << "'wooled' is in list: " << w2.contains("wooled") << std::endl;
	std::cout << "'WoOleD' is in list: " << w2.contains("WoOleD") << std::endl;
	std::cout << "'yerba' is in list: " << w2.contains("yerba") << std::endl;
	std::cout << "'asdfasdf' is in list: " << w2.contains("asdfasdf") << std::endl;
	std::cout << "'quidka' is in list: " << w2.contains("quidka") << std::endl;
	*/

	/*	tests loadWordList(), contains(), findCandidates()		7 WORKS ON BOTH COMPILERS
	WordListImpl w3;
	if (!w3.loadWordList("wordlist.txt"))
	{
		std::cout << "Unable to load word list" << std::endl;
		return 0;
	}
	if (w3.contains("onomatopoeia"))
		std::cout << "I found onomatopoeia!" << std::endl;
	else
		std::cout << "Onomatopoeia is not in the word list!" << std::endl;
	std::string cipher = "xyqbbq";
	std::string decodedSoFar = "?r????";
	std::vector<std::string> v = w3.findCandidates(cipher, decodedSoFar);
	if (v.empty())
		std::cout << "No matches found" << std::endl;
	else
	{
		std::cout << "Found these matches:" << std::endl;
		for (size_t k = 0; k < v.size(); k++)
			std::cout << v[k] << std::endl; // writes grotto and troppo
	}
	*/

	// Translator Tests ///////////////
	/*	tests constructor										8 WORKS ON BOTH COMPILERS
	TranslatorImpl t1;
	t1.printCurrMapping();
	*/

	/*	test pushMapping()	, getTranslation()					9 WORKS ON BOTH COMPILERS
	TranslatorImpl t;
	t.pushMapping("DHL", "ERD"); // DàE, HàR, LàD
	if (!t.pushMapping("QX", "RY")) // QàR, XàY
		std::cout << "Both H and Q would map to R!" << std::endl;
	// The current mapping is still DàE, HàR, LàD with no
	// mapping for Q or X
	std::cout << t.getTranslation("HDX") << std::endl; // writes RE?
	if (!t.pushMapping("H", "S")) // HàS
		std::cout << "H would map to both R and S!" << std::endl;
	*/

	/*	tests popMapping(), getTranslation()					10 WORKS ON BOTH COMPILERS
	//ahci ho cppw! -> Nick is COOL!
	TranslatorImpl t2;
	std::cout << t2.getTranslation("Ahci ho CPPW!") << std::endl;
	t2.pushMapping("AHCI", "niCk");
	std::cout << t2.getTranslation("Ahci ho CPPW!") << std::endl;
	t2.pushMapping("op", "sO");
	std::cout << t2.getTranslation("Ahci ho CPPW!") << std::endl;
	t2.pushMapping("a", "f");
	std::cout << t2.getTranslation("Ahci ho CPPW!") << std::endl;
	t2.pushMapping("z", "N");
	std::cout << t2.getTranslation("Ahci ho CPPW!") << std::endl;
	t2.pushMapping("w", "l");
	std::cout << t2.getTranslation("Ahci ho CPPW!") << std::endl;
	while (t2.popMapping())
		std::cout << t2.getTranslation("Ahci ho CPPW!") << std::endl;
	*/
	//11 tested shift to privided.h

// Decrypter Tests ///////////////							FINAL WORKS ON BOTH COMPILERS
	Decrypter d;
	if (!d.load("wordlist.txt"))
		std::cout << "dictionary failed to load" << std::endl;
	else
		std::cout << "dictionary loaded properly!" << std::endl;
	std::cout << std::endl;

	std::vector<std::string> v;

	/*
		//1
		v = d.crack("y qook ra bdttook yqkook");
		for (unsigned int i = 0; i < v.size(); i++)
			std::cout << v[i] << std::endl;
		std::cout << std::endl;

		//2
		v = d.crack("Vxgvab sovi jh pjhk cevc andi ngh iobnxdcjnh cn bdttook jb pnio jpfnicvhc cevh vha nceoi nho cejhy.");
		for (unsigned int i = 0; i < v.size(); i++)
			std::cout << v[i] << std::endl;
		std::cout << std::endl;
//
		//3
		v = d.crack("Trcy oyc koon oz rweelycbb vmobcb, wyogrcn oecyb; hjg ozgcy tc moox bo moya wg grc vmobck koon grwg tc ko yog bcc grc oyc trlvr rwb hccy oecyck zon jb. -Rcmcy Xcmmcn");
		for (unsigned int i = 0; i < v.size(); i++)
			std::cout << v[i] << std::endl;
		std::cout << std::endl;
	//
	//
		//4
		v = d.crack("Jxwpjq qwrla glcu pcx qcn xkvv dw uclw ekarbbckpjwe dq jzw jzkpta jzrj qcn ekep'j ec jzrp dq jzw cpwa qcn eke ec. -Urls Jxrkp");
		for (unsigned int i = 0; i < v.size(); i++)
			std::cout << v[i] << std::endl;
		std::cout << std::endl;
	//
	//
		//5
		v = d.crack("Axevfvu lvnelvp bxqp mvpprjv rgl bvoop Grnxvgkvuj dqupb jvbp buvrbvl be lqggvu.");
		for (unsigned int i = 0; i < v.size(); i++)
			std::cout << v[i] << std::endl;
		std::cout << std::endl;
*/

		//6
		v = d.crack("Xjzwq gjz cuvq xz huri arwqvudiy fuk ufjrqoq svquxiy. -Lzjk Nqkkqcy");
		for (int i = 0; i < v.size(); i++)
			std::cout << v[i] << std::endl;
		std::cout << std::endl;
//*/
/*
	v = d.crack("asdf qwer zxcv aals pekw");
	for (int i = 0; i < v.size(); i++)
		std::cout << v[i] << std::endl;
	std::cout << std::endl;
	*/
}

