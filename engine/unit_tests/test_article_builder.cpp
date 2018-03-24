#define BOOST_TEST_MODULE "test_article_builder"

#include "../lsh_deduplication.h"
#include "../article_builder.h"
#include "../config.h"
#include "article_utils.h"
#include "boost/test/included/unit_test.hpp"

using namespace boost::unit_test;
using namespace boost;
using namespace std;

BOOST_AUTO_TEST_CASE(test_article_from_xml)
{
	std::vector<std::pair<std::string, Signature>> docs_signatures;
	ArticleBuilder article_builder(docs_signatures, 0.3);
	vector<Article> articles;
	auto articles_xml = load_articles_xml("articles.xml");
	for(const auto& article_xml : articles_xml)
	{
		Article new_article;
		auto result = article_builder.from_xml(article_xml, new_article);
		if(result == BuilderRes::VALID || result == BuilderRes::DUPLICATE)
		{
			articles.push_back(new_article);
		}
		//else
			//cout << "title: " << new_article.title << endl;
	}
	cout << "articles: " << articles_xml.size() << " valid articles: " << articles.size() << "\n";
	for(const auto& article : articles)
	{
		BOOST_REQUIRE(article.length > 1000);
		BOOST_REQUIRE(article.words_no >= article.unknown_words_no);
		size_t words_sum = 0;
		for(size_t idx = 0; idx < article.tf.size(); idx++)
		{
			words_sum += article.tf[idx];
		}
		BOOST_CHECK_EQUAL(words_sum, article.words_no - article.unknown_words_no);
	}
}

BOOST_AUTO_TEST_CASE(test_articles_signatures)
{
	std::vector<std::pair<std::string, Signature>> docs_signatures;
	ArticleBuilder article_builder(docs_signatures, 0.15);
	vector<Article> articles;
	auto articles_xml = load_articles_xml("articles.xml");
	vector<size_t> articles_idxs;
	size_t count = 0;
	for(const auto& article_xml : articles_xml)
	{
		Article new_article;
		auto result = article_builder.from_xml(article_xml, new_article);
		if(result == BuilderRes::VALID || result == BuilderRes::DUPLICATE)
		{
			articles.push_back(new_article);
			articles_idxs.push_back(count);
		}
		count++;
	}

	//check if same articles have the same signatures
	cout << "80 articles: " << articles_xml.size() << " valid articles: " << articles.size() << "\n";
	for(size_t idx = 0; idx < articles.size(); idx++)
	{
		Article new_article;
		auto result = article_builder.from_xml(articles_xml[articles_idxs[idx]], new_article);
		BOOST_REQUIRE(result == BuilderRes::VALID || result == BuilderRes::DUPLICATE);
		BOOST_CHECK(new_article.signature == articles[idx].signature);
		BOOST_CHECK(new_article.tf == articles[idx].tf);
	}
	//articles 59 and 65 are identical
	BOOST_REQUIRE(articles[59].signature == articles[65].signature);
}

BOOST_AUTO_TEST_CASE(test_lsh_deduplication)
{
	std::vector<std::pair<std::string, Signature>> docs_signatures;
	ArticleBuilder article_builder(docs_signatures, 0.15);
	vector<Article> articles;
	auto articles_xml = load_articles_xml("articles.xml");
	vector<size_t> articles_idxs;
	for(const auto& article_xml : articles_xml)
	{
		Article new_article;
		auto result = article_builder.from_xml(article_xml, new_article);
		if(result == BuilderRes::VALID || result == BuilderRes::DUPLICATE)
		{
			articles.push_back(new_article);
			docs_signatures.push_back(make_pair(new_article.title, new_article.signature));
		}
	}
	LSHDeduplication lsh_deduplication(SIGNATURE_SIZE, docs_signatures);
	docs_signatures[65].first = "new title";
	for(size_t idx = 0; idx < docs_signatures.size(); idx++)
	{
		docs_signatures[idx].first += "second";
		auto duplicates = lsh_deduplication.process_doc(docs_signatures[idx]);
		if(idx != 65)
			BOOST_REQUIRE_EQUAL(duplicates.size(), 2);
		else
			BOOST_REQUIRE_EQUAL(duplicates.size(), 3);
	}
}

