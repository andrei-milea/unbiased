#include "mongodb.h"
#include <bsoncxx/json.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/stdx.hpp>
#include <mongocxx/uri.hpp>
#include <bsoncxx/types.hpp>
#include <bsoncxx/types/value.hpp>
#include <bsoncxx/stdx/string_view.hpp>

using namespace std;
using namespace mongocxx;
using bsoncxx::builder::stream::document;
using bsoncxx::builder::stream::finalize;

std::map<std::string, WordInfo> MongoDb::load_vocabulary_map()const
{
	collection vocab_collection = database_["vocabulary"];
	bsoncxx::stdx::optional<bsoncxx::document::value> result = vocab_collection.find_one(document{} << "language" << "english" << finalize);
	if(!result)
		throw std::runtime_error("failed to find english vocabulary in mongodb");

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
		throw std::runtime_error("failed to find english vocabulary in mongodb");
	unordered_set<string> stop_words;
	bsoncxx::document::element elem{(*result).view()["stop_words"]};
	bsoncxx::array::view words_array{elem.get_array().value};
	for(const bsoncxx::array::element& word_elem : words_array)
		stop_words.insert(word_elem.get_utf8().value.to_string());
	return stop_words;
}

std::vector<Article> MongoDb::load_articles()const
{
	std::vector<Article> articles;
}

uint64_t MongoDb::get_articles_no()const
{
	return 3;
}

