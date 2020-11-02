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
        set<string> stems;
        bool valid = article_parser.tokenize_validate(new_article, stems);
        if (valid)
        {
            vocab.add_stems(std::move(stems));
            articles.push_back(std::move(new_article));
        }
    }

    log_runtime.log("parsed articles");
    spdlog::info("articles: {} valid articles: {}", articles_xml.size(), articles.size());


    vector<ProcessedArticle> processed_articles;
    for (auto && article : articles)
    {
        ProcessedArticle proc_article;
        article_parser.process_tokens(article, proc_article);
        processed_articles.push_back(std::move(proc_article));
    }
    log_runtime.log("processed articles");

    BU_Clustering clust_proc { vocab };
    clust_proc.create_clusters(processed_articles);
    log_runtime.log("create_clusters");
}
