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
    ArticleParser article_parser{ vocab };
    auto articles_xml = load_articles_xml("articles.xml");
    std::unordered_set<string> titles;
    std::vector<Article> articles;
    for (const auto& article_xml : articles_xml)
    {
        Article new_article;
        article_parser.parse_from_xml(article_xml, new_article);
        if (titles.count(new_article.meta_data.title) == 0)
        {
            titles.insert(new_article.meta_data.title);
            set<string> stems;
            bool valid = article_parser.tokenize_validate(new_article, stems);
            if (valid)
            {
                vocab.add_stems(std::move(stems));
                articles.push_back(std::move(new_article));
            }
        }
    }
    spdlog::info("articles: {} valid articles: {}", articles_xml.size(), articles.size());
    log_runtime.log("tokenized articles and loaded vocabulary");
    std::vector<ProcessedArticle> processed_articles;
    for (auto && article : articles)
    {
        ProcessedArticle proc_article;
        article_parser.process_tokens(article, proc_article);
        processed_articles.push_back(std::move(proc_article));
    }
    log_runtime.log("processed tokens");
    LSA lsa_processor { vocab };
    lsa_processor.run_svd(processed_articles);
    log_runtime.log("finished running svd");
    //lsa_processor.print_term_doc_matrix();
    //lsa_processor.print_sigma();
    //cout << "\n\n\n";
    //lsa_processor.print_top_concepts(articles, 0.12);
}
