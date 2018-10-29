#ifndef _ARTICLE_H
#define _ARTICLE_H

#include <iostream>
#include <string>
#include <vector>
#include <array>
#include <set>

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

	bool operator==(const Article& article)const
	{
		return /*id == article.id &&*/ url == article.url && title == article.title && source == article.source 
						&& authors == article.authors && length == article.length && words_no == article.words_no && signature == article.signature
						&& tf == article.tf && keywords == article.keywords && entities == article.entities /*&& duplicates == article.duplicates*/;
	}

	void print_tf()const
	{
		for(size_t idx = 0; idx < tf.size(); idx++)
			std::cout << tf[idx] << " ";
		std::cout << "\n";
	}

	std::string id;
	std::string url;
	std::string title;
	std::string date;
	std::string source;
	std::vector<std::string> authors;
	size_t length;
	size_t words_no;
	size_t unknown_words_no;
	Signature signature;
	std::vector<double> tf;
	std::vector<std::string> keywords;
	std::vector<std::string> entities;
	std::set<std::string> duplicates;
	Analytics analitycs;
};

#endif

