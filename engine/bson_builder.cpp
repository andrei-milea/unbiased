#include "bson_builder.h"
#include <bsoncxx/json.hpp>
#include <bsoncxx/types.hpp>
#include <bsoncxx/types/value.hpp>
#include <bsoncxx/stdx/string_view.hpp>
#include <bsoncxx/builder/stream/array.hpp>
#include <bsoncxx/builder/stream/helpers.hpp>
#include <string>
#include <utility>

using bsoncxx::builder::stream::finalize;
using bsoncxx::builder::stream::array_context;
using bsoncxx::builder::stream::open_array;
using bsoncxx::builder::stream::close_array;
using bsoncxx::builder::stream::open_document;
using bsoncxx::builder::stream::close_document;
using bsoncxx::builder::stream::finalize;
using namespace std;

bson_doc BsonBuilder::to_bson(const Vocabulary& vocab, bool only_freq)const
{
	bson_doc_bld bson_doc_builder;
	if(true == only_freq)
	{
		auto doc = bson_doc_builder << "words_freq" << open_array << [&vocab](array_context<> arr)
							{
								auto words_freq = vocab.get_words_freq();
								for(auto word_freq : words_freq)
									arr << (int64_t)word_freq;
							}	<< close_array << finalize;
		return doc;
	}
else
	{
		auto doc = bson_doc_builder << "words" << open_array << [&vocab](array_context<> arr)
							{
								for(const auto& word : vocab.get_words())
									arr << word;
							}	<< close_array
						 << "words_freq" << open_array << [&vocab](array_context<> arr)
							{
								auto words_freq = vocab.get_words_freq();
								for(auto word_freq : words_freq)
									arr << (int64_t)word_freq;
							}	<< close_array
						 << "stop_words" << open_array << [&vocab](array_context<> arr)
							{
								for(const auto& stop_word : vocab.get_stop_words())
									arr << stop_word;
							}	<< close_array
						 << finalize; 
		return doc;
	}
}

bson_doc BsonBuilder::to_bson(const Article &article)const
{
	bson_doc_bld bson_doc_builder;
	auto doc = bson_doc_builder << "url" << article.url
							 << "title" << article.title
							 << "date" << article.date
							 << "source" << article.source
							 << "authors" << open_array << [&article](array_context<> arr)
							 	{
									for (const auto& author : article.authors)
										arr << author;
							 	}	<< close_array
							<< "length" << int64_t(article.length)
							<< "words_no" << int64_t(article.words_no)
							<< "signature" << open_array << [&article](array_context<> arr)
							 	{
									for (auto elem : article.signature)
										arr << (int64_t)elem;
							 	}	<< close_array
							<< "tf" << open_array << [&article](array_context<> arr)
							 	{
									for (auto elem : article.tf)
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
	return doc;
}

void BsonBuilder::from_bson(const bson_doc_view &doc, Vocabulary& vocab)const
{
	bsoncxx::array::view words_array{doc["words"].get_array().value};
	for(const bsoncxx::array::element& word : words_array)
		vocab.words_.insert(word.get_utf8().value.to_string());

	bsoncxx::array::view freq_array{doc["words_freq"].get_array().value};
	size_t idx = 0;
	for(const bsoncxx::array::element& freq : freq_array)
	{
		vocab.words_freq_[idx] = freq.get_int64().value;
		idx++;
	}

	bsoncxx::array::view stop_words_array{doc["stop_words"].get_array().value};
	for(const bsoncxx::array::element& stop_word : stop_words_array)
		vocab.stop_words_.insert(stop_word.get_utf8().value.to_string());
}

void BsonBuilder::from_bson(const bson_doc_view &doc, Article &article)const
{
	article.id = doc["_id"].get_oid().value.to_string();
	article.url = doc["url"].get_utf8().value.to_string();
	article.title = doc["title"].get_utf8().value.to_string();
	article.date = doc["date"].get_utf8().value.to_string();
	article.length = doc["length"].get_int64().value;
	article.words_no = doc["words_no"].get_int64().value;
	article.source = doc["source"].get_utf8().value.to_string();

	bsoncxx::array::view authors_array{doc["authors"].get_array().value};
	for(const bsoncxx::array::element& auth_elem : authors_array)
		article.authors.push_back(auth_elem.get_utf8().value.to_string());

	bsoncxx::array::view sig_array{doc["signature"].get_array().value};
	int idx = 0;
	for(const bsoncxx::array::element& sig_elem : sig_array)
	{
		article.signature[idx] = (uint32_t)sig_elem.get_int64().value;
		idx++;
	}

	bsoncxx::array::view tf_array{doc["tf"].get_array().value};
	for(const bsoncxx::array::element& tf_elem : tf_array)
		article.tf.push_back(tf_elem.get_double().value);

	bsoncxx::array::view keywords_array{doc["keywords"].get_array().value};
	for(const bsoncxx::array::element& keyword_elem : keywords_array)
		article.keywords.push_back(keyword_elem.get_utf8().value.to_string());

	bsoncxx::array::view entities_array{doc["entities"].get_array().value};
	for(const bsoncxx::array::element& entity_elem : entities_array)
		article.entities.push_back(entity_elem.get_utf8().value.to_string());
}

void BsonBuilder::from_bson(const bson_doc_view &doc, std::pair<std::string, Signature>& art_sig)const
{
	art_sig.first = doc["_id"].get_oid().value.to_string();
	bsoncxx::array::view sig_array{doc["signature"].get_array().value};
	int idx = 0;
	for(const bsoncxx::array::element& sig_elem : sig_array)
	{
		art_sig.second[idx] = (uint32_t)sig_elem.get_int64().value;
		idx++;
	}
}

void BsonBuilder::from_bson(const bson_doc_view &doc, pair<string, string>& art_date)const
{
	art_date.first = doc["_id"].get_oid().value.to_string();
	art_date.second = doc["date"].get_utf8().value.to_string();
}

