#ifndef _BSON_BUILDER_H
#define _BSON_BUILDER_H

#include "vocabulary.h"
#include "article.h"
#include <bsoncxx/builder/stream/document.hpp>

using bson_doc_bld = bsoncxx::builder::stream::document;
using bson_doc = bsoncxx::document::value;
using bson_doc_view = bsoncxx::document::view;

//TODO - share same builder?
class BsonBuilder
{
public:

	bson_doc to_bson(const Vocabulary& vocab, bool only_freq = false)const;

	bson_doc to_bson(const Article &article)const;

	void from_bson(const bson_doc_view &doc, Vocabulary& vocab)const;

	void from_bson(const bson_doc_view &doc, Article &article)const;

	void from_bson(const bson_doc_view &doc, std::pair<std::string, Signature>& art_sig)const;

	void from_bson(const bson_doc_view &doc, std::pair<std::string, std::string>& art_date)const;
};

#endif


