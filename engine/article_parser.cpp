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
    article.meta_data.url = pt.get<std::string>("article.url");
    article.meta_data.title = pt.get<std::string>("article.title");
    article.meta_data.date = pt.get<std::string>("article.date");
    auto authors_node = pt.get_child("article.authors");
    for (const auto& author_node : authors_node)
        article.meta_data.authors.emplace_back(author_node.second.data());

    article.text = pt.get<std::string>("article.text");
}

bool ArticleParser::tokenize_validate(Article& article, std::set<std::string> &valid_stems, int32_t min_tokens, float invalid_tokens_threshold)
{
    article.tokens = tokenize(article.text);
    if (article.tokens.size() < min_tokens)
        return false;

    int32_t unknown_tokens_no = 0;
    for (const auto& token : article.tokens)
    {
        if (vocabulary_.is_valid_word(token))
        {
            std::string stem = boost::locale::to_lower(token);
            trim_and_stem(stem);
            valid_stems.insert(stem);
        }
        else if(!vocabulary_.is_stop_word(token) && token.size() > 1)
            unknown_tokens_no++;
    }
    return float(unknown_tokens_no + 1.0) / article.tokens.size() <= invalid_tokens_threshold;
}
