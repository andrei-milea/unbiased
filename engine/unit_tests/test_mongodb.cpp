#define BOOST_TEST_MODULE "test_mongodb"
#define protected public

#include "article.h"
#include "article_parser.h"
#include "mongodb_helper.h"
#include "config.h"
#include "log_helper.h"
#include "bson_builder.h"
#include "mongodb.h"
#include "article_utils.h"
#include "stl_ext.h"
#include "boost/test/included/unit_test.hpp"
#include <chrono>
#include <vector>

using namespace boost::unit_test;
using namespace std;
using namespace bson_bld;

RegisterUnitTestLogger register_logger{BOOST_TEST_MODULE};

const string words_filename{"words.dat"};
const string stopwords_filename{"stop_words.dat"};

static std::vector<ProcessedArticle> load_articles(MongoDb& db_inst)
{
    auto result = db_inst.get_docs("articles");
    std::vector<ProcessedArticle> articles;
    for (const auto& doc : result)
    {
        ProcessedArticle article;
        from_bson(doc, article);
        articles.push_back(article);
    }
    return articles;
}

BOOST_AUTO_TEST_CASE(test_vocabulary_db)
{
    MongoDb db_inst{"test"};
    //run this first time
    db_inst.drop_collection("vocabulary");
    Vocabulary vocab { words_filename, stopwords_filename };
    auto doc = to_bson(vocab);
    db_inst.save_doc("vocabulary", doc);

    Vocabulary new_vocab;
    auto result = db_inst.get_docs("vocabulary");
    from_bson(*(result.begin()), new_vocab);

    BOOST_REQUIRE(vocab.stems_no() == new_vocab.stems_no());
}

BOOST_AUTO_TEST_CASE(test_save_articles)
{
    spdlog::info("reading vocabulary from db");
    MongoDb db_inst{"test"};
    Vocabulary init_vocab;
    auto result = db_inst.get_docs("vocabulary");
    bson_bld::from_bson(*(result.begin()), init_vocab);

    auto articles_xml = load_articles_xml("articles.xml");

    spdlog::info("building articles: {}", articles_xml.size());
    ArticleParser article_parser{init_vocab};
    vector<Article> articles;
    for (const auto& article_xml : articles_xml)
    {
        Article new_article;
        article_parser.parse_from_xml(article_xml, new_article);
        set<string> stems;
        bool valid = article_parser.tokenize_validate(new_article, stems);
        if (valid)
        {
            init_vocab.add_stems(std::move(stems));
            articles.push_back(std::move(new_article));
        }
    }
    vector<ProcessedArticle> processed_articles;
    for (auto&& article : articles)
    {
        ProcessedArticle proc_article;
        article_parser.process_tokens(article, proc_article);
        processed_articles.push_back(std::move(proc_article));
    }

    db_inst.drop_collection("articles");
    std::set<string> generated_ids;
    for (const auto& proc_article : processed_articles)
    {
        auto doc = bson_bld::to_bson(proc_article);
        string id = db_inst.save_doc("articles", doc);
        generated_ids.insert(id);
    }

    vector<string> ids{generated_ids.begin(), generated_ids.end()};
    auto articles_signatures = load_articles_signatures(db_inst);
    for (const auto& article_signature : articles_signatures)
    {
        auto it = generated_ids.find(article_signature.first);
        BOOST_REQUIRE(it != generated_ids.end());
    }

    auto articles_db = load_articles(db_inst);
    BOOST_REQUIRE_EQUAL(articles_db, processed_articles);

    db_inst.drop_collection("articles");
}

