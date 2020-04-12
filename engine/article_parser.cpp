#include "article_parser.h"
#include "utils/tokenize.h"
#include <boost/locale.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <vector>

using boost::property_tree::ptree;
using namespace std;

ArticleParser::ArticleParser(const Vocabulary &vocab)
    : vocabulary_(vocab)
    , min_hash_(SIGNATURE_SIZE, MINHASH_RANGE_MAX)
{
    //create system default locale
    boost::locale::generator gen;
    std::locale loc = gen("");
    //make it system global
    std::locale::global(loc);
}

void ArticleParser::parse_from_xml(const std::string& article_xml, Article& article)
{
    ptree pt;
    std::stringstream ss;
    ss << article_xml;
    read_xml(ss, pt);
    article.url = pt.get<std::string>("article.url");
    article.title = pt.get<std::string>("article.title");
    article.date = pt.get<std::string>("article.date");
    auto authors_node = pt.get_child("article.authors");
    for (const auto& author_node : authors_node)
        article.authors.emplace_back(author_node.second.data());

    std::string text = pt.get<std::string>("article.text");
    article.length = text.size();
    article.tokens = tokenize(text);
}
