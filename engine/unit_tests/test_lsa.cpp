#define BOOST_TEST_MODULE "test_lsa"

#include "../vocabulary.h"
#include "../lsa.h"
#include "../article_parser.h"
#include "../utils/article_utils.h"
#include "../utils/log_helper.h"
#include "../utils/perf_clock.h"

#include "boost/test/included/unit_test.hpp"
#include <fstream>

using namespace boost::unit_test;
using namespace boost;
using namespace std;

RegisterUnitTestLogger register_logger(BOOST_TEST_MODULE);

const string words_filename{"words.dat"};
const string stopwords_filename{"stop_words.dat"};

BOOST_AUTO_TEST_CASE(test_lsa_svd)
{
    LogRunTime log_runtime("test_lsa_svd");
    Vocabulary vocab { words_filename, stopwords_filename };
    ArticleParser article_parser{vocab};
    vector<Article> articles;
    auto articles_xml = load_articles_xml("articles.xml");
    for (const auto& article_xml : articles_xml)
    {
        Article new_article;
        article_parser.parse_from_xml(article_xml, new_article);
        if (new_article.is_valid())
        {
            spdlog::info("valid article {}", new_article.tokens.size());
            article_parser.process_tokens(new_article);
            articles.push_back(new_article);
        }
        else
            spdlog::info("invalid article title: {}", new_article.title);
    }
    log_runtime.log("built articles");
    spdlog::info("articles: {} valid articles: {}", articles_xml.size(), articles.size());
    LSA lsa_processor { vocab };
    lsa_processor.run_svd(articles);
    log_runtime.log("finished running svd");
    //lsa_processor.print_term_doc_matrix();
    //lsa_processor.print_sigma();
    //cout << "\n\n\n";
    //lsa_processor.print_top_concepts(articles, 0.12);
}
