#include "mongodb_helper.h"
#include "article.h"
#include <bsoncxx/builder/stream/array.hpp>
#include <bsoncxx/builder/stream/helpers.hpp>
#include <bsoncxx/json.hpp>
#include <bsoncxx/stdx/string_view.hpp>
#include <bsoncxx/types.hpp>
#include <bsoncxx/types/value.hpp>

using bsoncxx::builder::stream::array_context;
using bsoncxx::builder::stream::close_array;
using bsoncxx::builder::stream::close_document;
using bsoncxx::builder::stream::finalize;
using bsoncxx::builder::stream::open_array;
using bsoncxx::builder::stream::open_document;

using namespace std;

vector<pair<string, string>> load_articles_dates(MongoDb& mongodb_inst, const vector<string>& articles_ids)
{
    vector<pair<string, string>> articles_dates;
    bson_doc_bld filter_doc;
    filter_doc << "$or" << open_array << [&articles_ids](array_context<> arr) {
        for (const auto& article_id : articles_ids)
            arr << open_document << "id" << article_id << close_document;
    } << close_array;

    vector<string> fields({ "_id", "date" });
    auto result = mongodb_inst.get_fields("articles", fields, filter_doc);
    for (const auto& doc : result)
    {
        pair<string, string> art_date;
        bson_bld::from_bson(doc, art_date);
        articles_dates.push_back(art_date);
    }
    return articles_dates;
}

vector<pair<string, Signature>> load_articles_signatures(MongoDb& mongodb_inst)
{
    vector<pair<string, Signature>> articles_signatures;
    vector<string> fields({ "_id", "signature" });
    auto result = mongodb_inst.get_fields("articles", fields);
    for (const auto& doc : result)
    {
        pair<string, Signature> article_signature;
        bson_bld::from_bson(doc, article_signature);
        articles_signatures.push_back(article_signature);
    }
    return articles_signatures;
}
