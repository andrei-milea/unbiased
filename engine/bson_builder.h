#ifndef _BSON_BUILDER_H
#define _BSON_BUILDER_H

#include <bsoncxx/builder/stream/document.hpp>
#include "article.h"

using bson_doc_bld = bsoncxx::builder::stream::document;
using bson_doc = bsoncxx::document::value;
using bson_doc_view = bsoncxx::document::view;

class Vocabulary;
class Article;

namespace bson_bld
{

bson_doc to_bson(const Vocabulary& vocab, bool only_freq = false);

bson_doc to_bson(const Article& article);

void from_bson(const bson_doc_view& doc, Vocabulary& vocab);

void from_bson(const bson_doc_view& doc, Article& article);

void from_bson(const bson_doc_view& doc, std::pair<std::string, Signature>& art_sig);

void from_bson(const bson_doc_view& doc, std::pair<std::string, std::string>& art_date);

}

#endif
