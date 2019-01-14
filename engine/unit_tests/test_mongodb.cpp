#define BOOST_TEST_MODULE "test_mongodb"
#define protected public

#include "../mongodb.h"
#include "../config.h"
#include "../utils/article_utils.h"
#include <vector>
#include <chrono>
#include "boost/test/included/unit_test.hpp"

using namespace boost::unit_test;
using namespace std;

const string words_filename("words.dat");
const string stopwords_filename("stop_words.dat");

BOOST_AUTO_TEST_CASE(test_vocabulary_db)
{
	//run this first time
	auto& mongo_inst = ArticleBuilder::db_inst_;
	mongo_inst.drop_collection("vocabulary");
	Vocabulary vocab{words_filename, stopwords_filename};
	BsonBuilder bson_bld;
	auto doc = bson_bld.to_bson(vocab);
	ArticleBuilder::db_inst_.save_doc("vocabulary", doc);

	Vocabulary new_vocab;
	auto result = ArticleBuilder::db_inst_.get_docs("vocabulary");
	bson_bld.from_bson(*(result.begin()), new_vocab);

	BOOST_REQUIRE(vocab.words_no() == new_vocab.words_no());
	BOOST_REQUIRE(vocab.stopwords_no() == new_vocab.stopwords_no());
	BOOST_REQUIRE(new_vocab.stopwords_no() == 153);
	BOOST_REQUIRE(new_vocab.words_no() == 152588);
}

BOOST_AUTO_TEST_CASE(test_save_articles)
{
	cout << "reading vocabulary from db" << endl;
	auto start = std::chrono::high_resolution_clock::now();
	Vocabulary init_vocab;
	BsonBuilder bson_bld;
	auto result = ArticleBuilder::db_inst_.get_docs("vocabulary");
	bson_bld.from_bson(*(result.begin()), init_vocab);
	auto finish = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double> elapsed = finish - start;
	cout << "elapsed time: " << elapsed.count() << "s" << endl;

	auto& mongo_inst = ArticleBuilder::db_inst_;
	ArticleBuilder article_builder(0.15);
	vector<Article> articles;
	auto articles_xml = load_articles_xml("articles.xml");
	cout << "building articles: " << articles_xml.size() << endl;
	start = std::chrono::high_resolution_clock::now();
	std::set<Signature> signatures;
	for(const auto& article_xml : articles_xml)
	{
		Article new_article;
		auto result = article_builder.from_xml(article_xml, new_article);
		if(result == BuilderRes::VALID || result == BuilderRes::DUPLICATE)
		{
			articles.push_back(new_article);
			signatures.insert(new_article.signature);
		}
	}
	finish = std::chrono::high_resolution_clock::now();
	elapsed = finish - start;
	cout << "elapsed time: " << elapsed.count() << "s" << endl;

	for(size_t idx = 0; idx < init_vocab.words_no(); idx++)
		BOOST_REQUIRE(init_vocab.get_word_freq(idx)  <= article_builder.get_vocabulary().get_word_freq(idx));

	mongo_inst.drop_collection("articles");
	for(const auto& article : articles)
	{
		BsonBuilder bson_bld;
		auto doc = bson_bld.to_bson(article);
		mongo_inst.save_doc("articles", doc);
	}

	//auto articles_no = mongo_inst.get_docs_no("articles");
	//BOOST_REQUIRE_EQUAL(articles_no, articles.size());

	ArticleBuilder art_bld;
	auto articles_signatures = art_bld.load_articles_signatures();
	for(const auto& article_signature : articles_signatures)
	{
		auto it = signatures.find(article_signature.second);
		BOOST_REQUIRE(it != signatures.end());
	}


	auto articles_db = art_bld.load_articles();
	BOOST_REQUIRE(articles_db == articles);

	mongo_inst.drop_collection("articles");
}

BOOST_AUTO_TEST_CASE(test_articles_duplicates)
{
	std::vector<std::pair<std::string, Signature>> docs_signatures;
	ArticleBuilder article_builder(0.15);
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
	auto& mongo_inst = ArticleBuilder::db_inst_;
	mongo_inst.drop_collection("articles");
	for(const auto& article : articles)
	{
		BsonBuilder bson_bld;
		auto doc = bson_bld.to_bson(article);
		mongo_inst.save_doc("articles", doc);
	}

	auto articles_signatures = article_builder.load_articles_signatures();
	ArticleBuilder new_builder(0.15);
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

