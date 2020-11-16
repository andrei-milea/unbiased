#include "bson_builder.h"
#include "vocabulary.h"
#include <bsoncxx/builder/stream/array.hpp>
#include <bsoncxx/builder/stream/helpers.hpp>
#include <bsoncxx/json.hpp>
#include <bsoncxx/stdx/string_view.hpp>
#include <bsoncxx/types.hpp>
#include <bsoncxx/types/value.hpp>
#include <string>
#include <utility>

using bsoncxx::builder::stream::array_context;
using bsoncxx::builder::stream::close_array;
using bsoncxx::builder::stream::close_document;
using bsoncxx::builder::stream::finalize;
using bsoncxx::builder::stream::open_array;
using bsoncxx::builder::stream::open_document;
using namespace std;

namespace bson_bld
{

bson_doc to_bson(const Vocabulary& vocab, bool only_freq)
{
    bson_doc_bld bson_doc_builder;
    if (true == only_freq)
    {
        auto doc = bson_doc_builder << "stems_freq" << open_array <<
            [&vocab](array_context<> arr) {
                auto words_freq = vocab.get_stems_freq();
                for (auto word_freq : words_freq)
                    arr << (int64_t)word_freq;
            } << close_array << finalize;
        return doc;
    }
    else
    {
        auto doc = bson_doc_builder << "stems" << open_array <<
            [&vocab](array_context<> arr) {
                for (const auto& word : vocab.get_stems())
                    arr << word;
            } << close_array
                                    << "stems_freq" << open_array <<
            [&vocab](array_context<> arr) {
                auto words_freq = vocab.get_stems_freq();
                for (auto word_freq : words_freq)
                    arr << (int64_t)word_freq;
            } << close_array
                                    << "stop_words" << open_array <<
            [&vocab](array_context<> arr) {
                for (const auto& stop_word : vocab.get_stop_words())
                    arr << stop_word;
            } << close_array << finalize;
        return doc;
    }
}

bson_doc to_bson(const ProcessedArticle& article)
{
    bson_doc_bld bson_doc_builder;
    auto doc = bson_doc_builder << "url" << article.meta_data.url
                                << "title" << article.meta_data.title
                                << "date" << article.meta_data.date
                                << "source" << article.meta_data.source
                                << "unknown_tokens_no" << article.unknown_tokens_no
                                << "authors" << open_array <<
        [&article](array_context<> arr) {
            for (const auto& author : article.meta_data.authors)
                arr << author;
        } << close_array
                                << "length" << int64_t(article.length)
                                << "signature" << open_array << [&article](array_context<> arr) {
                                       for (auto elem : article.signature)
                                           arr << (int64_t)elem;
                                   }
                                << close_array << "stem_ids" << open_array << [&article](array_context<> arr) {
                                       for (auto elem : article.stem_ids)
                                           arr << elem;
                                   }
                                << close_array << "entities" << open_array << [&article](array_context<> arr) {
                                       for (const auto& elem : article.entities)
                                           arr << elem;
                                   }
                                << close_array << finalize;
    return doc;
}

void from_bson(const bson_doc_view& doc, Vocabulary& vocab)
{
    bsoncxx::array::view words_array { doc["stems"].get_array().value };
    set<string> stems;
    for (const bsoncxx::array::element& word : words_array)
        stems.insert(word.get_utf8().value.to_string());
    vocab.add_stems(std::move(stems));
}

void from_bson(const bson_doc_view& doc, ProcessedArticle& article)
{
    article.meta_data.id = doc["_id"].get_oid().value.to_string();
    article.meta_data.url = doc["url"].get_utf8().value.to_string();
    article.meta_data.title = doc["title"].get_utf8().value.to_string();
    article.meta_data.date = doc["date"].get_utf8().value.to_string();
    article.meta_data.source = doc["source"].get_utf8().value.to_string();
    article.length = doc["length"].get_int64().value;

    bsoncxx::array::view authors_array { doc["authors"].get_array().value };
    for (const bsoncxx::array::element& auth_elem : authors_array)
        article.meta_data.authors.push_back(auth_elem.get_utf8().value.to_string());

    bsoncxx::array::view sig_array { doc["signature"].get_array().value };
    int idx = 0;
    for (const bsoncxx::array::element& sig_elem : sig_array)
    {
        article.signature[idx] = (uint32_t)sig_elem.get_int64().value;
        idx++;
    }

    bsoncxx::array::view stem_ids_array { doc["stem_ids"].get_array().value };
    for (const bsoncxx::array::element& stem_id : stem_ids_array)
        article.stem_ids.push_back(stem_id.get_double().value);

    bsoncxx::array::view entities_array { doc["entities"].get_array().value };
    for (const bsoncxx::array::element& entity_elem : entities_array)
        article.entities.push_back(entity_elem.get_utf8().value.to_string());
}

void from_bson(const bson_doc_view& doc, std::pair<std::string, Signature>& art_sig)
{
    art_sig.first = doc["_id"].get_oid().value.to_string();
    bsoncxx::array::view sig_array { doc["signature"].get_array().value };
    int idx = 0;
    for (const bsoncxx::array::element& sig_elem : sig_array)
    {
        art_sig.second[idx] = (uint32_t)sig_elem.get_int64().value;
        idx++;
    }
}

void from_bson(const bson_doc_view& doc, pair<string, string>& art_date)
{
    art_date.first = doc["_id"].get_oid().value.to_string();
    art_date.second = doc["date"].get_utf8().value.to_string();
}

}
