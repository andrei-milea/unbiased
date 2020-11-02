#define BOOST_TEST_MODULE "test_article_parser"

#include "../article_parser.h"
#include "../utils/article_utils.h"
#include "../utils/log_helper.h"
#include "boost/test/included/unit_test.hpp"

using namespace boost::unit_test;
using namespace boost;
using namespace std;

RegisterUnitTestLogger register_logger{BOOST_TEST_MODULE};
const string words_filename{"words.dat"};
const string stopwords_filename{"stop_words.dat"};
constexpr float valid_article_coeff = 0.3;

BOOST_AUTO_TEST_CASE(test_articles_from_xml)
{
    Vocabulary vocab { words_filename, stopwords_filename };
    ArticleParser article_parser { vocab };
    vector<Article> articles;
    auto articles_xml = load_articles_xml("articles.xml");
    for (const auto& article_xml : articles_xml)
    {
        Article new_article;
        article_parser.parse_from_xml(article_xml, new_article);
        set<string> stems;
        bool valid = article_parser.tokenize_validate(new_article, stems);
        if (valid)
        {
            vocab.add_stems(std::move(stems));
            articles.push_back(std::move(new_article));
        }
    }
    spdlog::info("articles: {} valid articles: {}", articles_xml.size(), articles.size());

    vector<ProcessedArticle> processed_articles;
    for (auto&& article : articles)
    {
        ProcessedArticle proc_article;
        article_parser.process_tokens(article, proc_article);
        processed_articles.push_back(std::move(proc_article));
    }
    for (const auto& article : processed_articles)
    {
        BOOST_REQUIRE(article.length > 1000);
        BOOST_REQUIRE(article.tokens_no >= article.unknown_tokens_no);
        BOOST_REQUIRE_LE(float(article.unknown_tokens_no) / article.tokens_no, valid_article_coeff);
    }
}
