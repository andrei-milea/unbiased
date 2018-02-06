#include "mongodb.h"
#include <bsoncxx/json.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/stdx.hpp>
#include <mongocxx/uri.hpp>
#include <bsoncxx/types.hpp>
#include <bsoncxx/types/value.hpp>
#include <bsoncxx/stdx/string_view.hpp>
#include <bsoncxx/builder/stream/array.hpp>
#include <bsoncxx/builder/stream/helpers.hpp>

using namespace std;
using namespace mongocxx;
using bsoncxx::builder::stream::document;
using bsoncxx::builder::stream::finalize;
using bsoncxx::builder::stream::array_context;
using bsoncxx::builder::stream::open_array;
using bsoncxx::builder::stream::close_array;
using bsoncxx::builder::stream::open_document;
using bsoncxx::builder::stream::close_document;
using bsoncxx::builder::stream::finalize;

std::map<std::string, WordInfo> MongoDb::load_vocabulary_map()const
{
	collection vocab_collection = database_["vocabulary"];
	bsoncxx::stdx::optional<bsoncxx::document::value> result = vocab_collection.find_one(document{} << "language" << "english" << finalize);
	if(!result)
		throw std::runtime_error("MongoDb::load_vocabulary_map error : failed to find english vocabulary in mongodb");

	std::map<std::string, WordInfo> vocabulary_map;
	bsoncxx::document::element elem{(*result).view()["words"]};
	assert(elem.type() == bsoncxx::type::k_array);
	bsoncxx::array::view words_array{elem.get_array().value};
	for(const bsoncxx::array::element& word_elem : words_array)
	{
		assert(word_elem.type() == bsoncxx::type::k_document);
		std::string word(word_elem.get_document().view()["word"].get_utf8().value.to_string());
		WordInfo word_info{(uint32_t)word_elem.get_document().view()["id"].get_int32(), (uint64_t)word_elem.get_document().view()["frequency"].get_int32()};
		vocabulary_map[word] = word_info;
	}

	return vocabulary_map;
}

unordered_set<string> MongoDb::load_vocabulary_stop_words()const
{
	collection vocab_collection = database_["vocabulary"];
	bsoncxx::stdx::optional<bsoncxx::document::value> result = vocab_collection.find_one(document{} << "language" << "english" << finalize);
	if(!result)
		throw std::runtime_error("MongoDb::load_vocabulary_stop_words error : failed to find english vocabulary in mongodb");
	unordered_set<string> stop_words;
	bsoncxx::document::element elem{(*result).view()["stop_words"]};
	bsoncxx::array::view words_array{elem.get_array().value};
	for(const bsoncxx::array::element& word_elem : words_array)
		stop_words.insert(word_elem.get_utf8().value.to_string());
	return stop_words;
}

void MongoDb::save_article(const Article& article)
{
	collection articles_collection = database_["articles"];
	auto doc = bson_builder_ << "url" << article.url
							 << "title" << article.title
							 << "date" << article.date
							 << "authors" << open_array << [&article](array_context<> arr)
							 	{
									for (const auto& author : article.authors)
										arr << author;
							 	}	<< close_array
							<< "length" << int64_t(article.length)
							<< "signature" << open_array << [&article](array_context<> arr)
							 	{
									for (auto elem : article.signature)
										arr << elem;
							 	}	<< close_array
							<< "tf" << open_array << [&article](array_context<> arr)
							 	{
									for (auto elem : article.tf)
										arr << elem;
							 	}	<< close_array
							<< "idf" << open_array << [&article](array_context<> arr)
							 	{
									for (auto elem : article.idf)
										arr << elem;
							 	}	<< close_array
							<< "keywords" << open_array << [&article](array_context<> arr)
							 	{
									for (const auto& elem : article.keywords)
										arr << elem;
							 	}	<< close_array 
							<< "entities" << open_array << [&article](array_context<> arr)
							 	{
									for (const auto& elem : article.entities)
										arr << elem;
							 	}	<< close_array
							<< finalize; 

	bsoncxx::stdx::optional<mongocxx::result::insert_one> result = articles_collection.insert_one(doc.view());
	if(!result)
		throw runtime_error("MongoDb::save_article error: failed to insert value in db: ");
}

vector<Article> MongoDb::load_articles(const std::string& key, const std::string& value)const
{
	vector<Article> articles;
	collection articles_collection = database_["articles"];
	cursor result = articles_collection.find(document{} << key << value << finalize);
	for(const auto& doc : result)
	{
		Article article;
		article.id = doc["_id"].get_utf8().value.to_string();
		article.url = doc["url"].get_utf8().value.to_string();
		article.title = doc["title"].get_utf8().value.to_string();
		article.date = doc["date"].get_utf8().value.to_string();
		article.length = doc["length"].get_int64().value;

		bsoncxx::array::view authors_array{doc["authors"].get_array().value};
		for(const bsoncxx::array::element& auth_elem : authors_array)
			article.authors.push_back(auth_elem.get_utf8().value.to_string());

		bsoncxx::array::view sig_array{doc["signature"].get_array().value};
		int idx = 0;
		for(const bsoncxx::array::element& sig_elem : sig_array)
		{
			article.signature[idx] = sig_elem.get_int64().value;
			idx++;
		}

		bsoncxx::array::view tf_array{doc["tf"].get_array().value};
		for(const bsoncxx::array::element& tf_elem : tf_array)
			article.tf.push_back(tf_elem.get_double().value);

		bsoncxx::array::view idf_array{doc["idf"].get_array().value};
		for(const bsoncxx::array::element& idf_elem : tf_array)
			article.idf.push_back(idf_elem.get_double().value);

		bsoncxx::array::view keywords_array{doc["keywords"].get_array().value};
		for(const bsoncxx::array::element& keyword_elem : keywords_array)
			article.keywords.push_back(keyword_elem.get_utf8().value.to_string());

		bsoncxx::array::view entities_array{doc["entities"].get_array().value};
		for(const bsoncxx::array::element& entity_elem : entities_array)
			article.entities.push_back(entity_elem.get_utf8().value.to_string());

		articles.push_back(article);
	}

	return articles;
}

std::vector<std::pair<std::string, std::string>> MongoDb::load_articles_dates(const std::vector<std::string> &articles_ids)const
{
	vector<pair<string, string>> articles_dates;
	collection articles_collection = database_["articles"];
	bsoncxx::builder::stream::document filter_builder;
	filter_builder << "$or" << open_array << [&articles_ids](array_context<> arr)
							 	{
									for (const auto& article_id : articles_ids)
										arr << article_id;
							 	}	<< close_array;
	mongocxx::options::find opts{};
	opts.projection(document{} << "id" << 1 << "date" << 1 << finalize);
	cursor result = articles_collection.find(filter_builder.view(), opts);
	for(const auto& doc : result)
	{
		string article_id = doc["id"].get_utf8().value.to_string();
		string article_date = doc["date"].get_utf8().value.to_string();
		articles_dates.push_back(make_pair(article_id, article_date));
	}
	return articles_dates;
}

vector<pair<string, Signature>> MongoDb::load_articles_signatures()const
{
	vector<pair<string, Signature>> articles_signatures;
	collection articles_collection = database_["articles"];
	mongocxx::options::find opts{};
	opts.projection(document{} << "id" << 1 << "signature" << 1 << finalize);
	cursor result = articles_collection.find(document{} << finalize, opts);
	for(const auto& doc : result)
	{
		string article_id = doc["_id"].get_utf8().value.to_string();
		Signature article_signature;
		bsoncxx::array::view sig_array{doc["signature"].get_array().value};
		int idx = 0;
		for(const bsoncxx::array::element& sig_elem : sig_array)
		{
			article_signature[idx] = sig_elem.get_int64().value;
			idx++;
		}
		articles_signatures.push_back(make_pair(article_id, article_signature));
	}
	return articles_signatures;
}

uint64_t MongoDb::get_articles_no()const
{
	collection articles_collection = database_["articles"];
	return articles_collection.count(document{} << finalize);
}

