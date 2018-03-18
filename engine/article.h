#ifndef _ARTICLE_H
#define _ARTICLE_H

#include <string>
#include <vector>
#include <array>

const int SIGNATURE_SIZE = 100;
const size_t MAX_WORDS_NO = 100000000;
typedef std::array<uint32_t, SIGNATURE_SIZE> Signature;

struct Analytics
{
	double importance_score;
	double accuracy_score;
};

class Article
{
public:
	Article()
	:length(0),
	words_no(0),
	unknown_words_no(0)
	{}

	std::string id;
	std::string source;
	std::string url;
	std::string title;
	std::string date;
	std::vector<std::string> authors;
	size_t length;
	size_t words_no;
	size_t unknown_words_no;
	Signature signature;
	std::vector<double> tf;
	std::vector<std::string> keywords;
	std::vector<std::string> entities;
	Analytics analitycs;
};

#endif

