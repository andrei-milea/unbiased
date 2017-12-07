#ifndef _ARTICLE_H
#define _ARTICLE_H

#include <string>
#include <vector>

struct ArticleMeta
{
	std::string url_;
	std::string title_;
	std::string date_;
	std::vector<std::string> authors_;
	size_t length_;
};


class Article : public ArticleMeta
{

private:
	uint64_t id_;
	std::vector<std::string> keywords_;
	std::vector<std::string> places_;
	std::vector<uint64_t> signature_;
};

#endif

