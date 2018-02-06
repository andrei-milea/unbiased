#ifndef _ARTICLE_H
#define _ARTICLE_H

#include <string>
#include <vector>

const int SIGNATURE_SIZE = 100;
typedef std::array<int64_t, SIGNATURE_SIZE> Signature;

struct Analytics
{
	double importance_score;
	double accuracy_score;
};

class Article
{
public:
	Article()
	:id(0),
	length(0)
	{}

	std::string id;
	std::string source;
	std::string url;
	std::string title;
	std::string date;
	std::vector<std::string> authors;
	size_t length;
	Signature signature;
	std::vector<double> tf;
	std::vector<double> idf;
	std::vector<std::string> keywords;
	std::vector<std::string> entities;
	Analytics analitycs;
};

#endif

