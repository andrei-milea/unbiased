#define BOOST_TEST_MODULE "test_mongodb"
#define protected public

#include "../article.h"
#include "../article_parser.h"
#include "../mongodb_helper.h"
#include "../config.h"
#include "../utils/log_helper.h"
#include "../bson_builder.h"
#include "../mongodb.h"
#include "../utils/article_utils.h"
#include "../utils/stl_ext.h"
#include "boost/test/included/unit_test.hpp"
#include <chrono>
#include <vector>

using namespace boost::unit_test;
using namespace std;
using namespace bson_bld;

RegisterUnitTestLogger register_logger{BOOST_TEST_MODULE};

const string words_filename{"words.dat"};
const string stopwords_filename{"stop_words.dat"};

static std::vector<Article> load_articles(MongoDb &db_inst)
{
    auto result = db_inst.get_docs("articles");
    std::vector<Article> articles;
    for (const auto& doc : result)
    {
        Article article;
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
    BOOST_REQUIRE(vocab.stopwords_no() == new_vocab.stopwords_no());
    BOOST_REQUIRE(new_vocab.stopwords_no() == 153);
    BOOST_REQUIRE(new_vocab.stems_no() == 152588);
}

BOOST_AUTO_TEST_CASE(test_save_articles)
{
    spdlog::info("reading vocabulary from db");
    MongoDb db_inst{"test"};
    Vocabulary init_vocab;
    auto result = db_inst.get_docs("vocabulary");
    bson_bld::from_bson(*(result.begin()), init_vocab);

    vector<Article> articles;
    auto articles_xml = load_articles_xml("articles.xml");

    spdlog::info("building articles: {}", articles_xml.size());
    std::set<string> generated_ids;

    ArticleParser article_parser{init_vocab};
    for (const auto& article_xml : articles_xml)
    {
        Article new_article;
        article_parser.parse_from_xml(article_xml, new_article);
        if (new_article.is_valid())
            articles.push_back(new_article);
    }

    db_inst.drop_collection("articles");
    for (const auto& article : articles)
    {
        auto doc = bson_bld::to_bson(article);
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
    BOOST_REQUIRE_EQUAL(articles_db, articles);

    db_inst.drop_collection("articles");
}

