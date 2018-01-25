#ifndef _ARTICLE_H
#define _ARTICLE_H

#include <string>
#include <vector>

const int SIGNATURE_SIZE = 10;
typedef std::array<uint32_t, SIGNATURE_SIZE> Signature;

struct MetaData
{
	std::string url;
	std::string title;
	std::string date;
	std::vector<std::string> authors;
	size_t length;
};

struct Similarity
{
	Signature signature;
	std::vector<double> tf;
	std::vector<double> idf;
};

struct Analytics
{
	double importance_score;
	double accuracy_score;
};

struct NLPInfo
{
	std::vector<std::string> keywords_;
	std::vector<std::string> places_;
	std::vector<std::string> entities_;
};


class Article
{
public:
	const MetaData& get_meta()const
	{
		return meta_;
	}

	friend class ArticleBuilder;
private:
	MetaData meta_;
	Analytics analytics_;
	Similarity similarity_;
	uint64_t id_;
};

#endif

