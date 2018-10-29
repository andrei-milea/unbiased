#define BOOST_TEST_MODULE "test_mongodb"

#include "../mongodb.h"
#include "../config.h"
#include "../utils/article_utils.h"
#include <vector>
#include "boost/test/included/unit_test.hpp"

using namespace boost::unit_test;
using namespace std;

BOOST_AUTO_TEST_CASE(test_save_articles)
{
	std::vector<std::pair<std::string, Signature>> docs_signatures;
	ArticleBuilder article_builder(docs_signatures, 0.15);
	vector<Article> articles;
	auto articles_xml = load_articles_xml("articles.xml");
	size_t count = 0;
	std::set<Signature> signatures;
	for(const auto& article_xml : articles_xml)
	{
		Article new_article;
		auto result = article_builder.from_xml(article_xml, new_article);
		if(result == BuilderRes::VALID || result == BuilderRes::DUPLICATE)
		{
			articles.push_back(new_article);
			signatures.insert(new_article.signature);
			count++;
		}
	}

	auto& mongo_inst = MongoDb::get("test_db");
	mongo_inst.drop_collection("articles");
	for(const auto& article : articles)
	{
		mongo_inst.save_article(article);
	}

	auto articles_no = mongo_inst.get_articles_no();
	BOOST_REQUIRE_EQUAL(articles_no, count);

	auto articles_signatures = mongo_inst.load_articles_signatures();
	for(const auto& article_signature : articles_signatures)
	{
		auto it = signatures.find(article_signature.second);
		BOOST_REQUIRE(it != signatures.end());
	}


	auto articles_db = mongo_inst.load_articles();
	BOOST_REQUIRE(articles_db == articles);

	mongo_inst.drop_collection("articles");
}

BOOST_AUTO_TEST_CASE(test_articles_duplicates)
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
	//save articles
	auto& mongo_inst = MongoDb::get("test_db");
	mongo_inst.drop_collection("articles");
	for(const auto& article : articles)
	{
		mongo_inst.save_article(article);
	}


	auto articles_signatures = mongo_inst.load_articles_signatures();
	ArticleBuilder new_builder(articles_signatures, 0.15);
	for(const auto& article_xml : articles_xml)
	{
		Article new_article;
		auto result = new_builder.from_xml(article_xml, new_article);
		BOOST_REQUIRE(result == BuilderRes::DUPLICATE || result == BuilderRes::INVALID_WORDS);
		if(result == BuilderRes::DUPLICATE)
			BOOST_REQUIRE(new_article.duplicates.size() != 1);
	}

	mongo_inst.drop_collection("articles");
}

