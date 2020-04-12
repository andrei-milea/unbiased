#define BOOST_TEST_MODULE "test_bu_clustering"

#include "../vocabulary.h"
#include "../bu_clustering.h"
#include "../article_parser.h"
#include "../utils/article_utils.h"
#include "../utils/log_helper.h"
#include "../utils/perf_clock.h"
#include "boost/test/included/unit_test.hpp"

using namespace boost::unit_test;
using namespace boost;
using namespace std;

RegisterUnitTestLogger register_logger(BOOST_TEST_MODULE);

const string words_filename{"words.dat"};
const string stopwords_filename{"stop_words.dat"};

BOOST_AUTO_TEST_CASE(test_clustering)
{
    LogRunTime log_runtime("test_clustering");
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
            article_parser.process_tokens(new_article);
            articles.push_back(new_article);
        }
    }
    spdlog::info("articles: {} valid articles: {}", articles_xml.size(), articles.size());
    log_runtime.log("built articles");

    BU_Clustering clust_proc { vocab };
    clust_proc.create_clusters(articles);
    log_runtime.log("create_clusters");
}
