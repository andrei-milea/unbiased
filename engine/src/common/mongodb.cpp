#include "mongodb.h"
#include <mongocxx/client.hpp>
#include <mongocxx/stdx.hpp>
#include <mongocxx/uri.hpp>

using namespace std;
using bsoncxx::builder::stream::finalize;
using mongocxx::collection;
using mongocxx::cursor;

mongocxx::instance MongoDb::inst_;

string MongoDb::save_doc(const string& collect_str, bson_doc doc)
{
    collection collect = database_[collect_str];
    bsoncxx::stdx::optional<mongocxx::result::insert_one> result = collect.insert_one(std::move(doc));
    if (!result)
        throw runtime_error("MongoDb::save_doc error: failed to insert value in db");

    assert((*result).inserted_id().type() == bsoncxx::type::k_oid);
    return (*result).inserted_id().get_oid().value.to_string();
}

mongocxx::cursor MongoDb::get_docs(const std::string& collect_str, const std::string& key, const std::string& value) const
{
    collection collect = database_[collect_str];
    cursor result = (key.empty() && value.empty()) ? collect.find(bson_doc_bld {} << finalize)
                                                   : collect.find(bson_doc_bld {} << key << value << finalize);
    return result;
}

void MongoDb::update_doc(const std::string& collect_str, const bson_doc& doc,
    const std::string& key, const std::string& value)
{
    collection collect = database_[collect_str];
    auto result = collect.update_one(bson_doc_bld {} << key << value << finalize, bson_doc_bld {} << "$set" << doc << finalize);
    if (!result)
        throw runtime_error("MongoDb::update_doc error: failed to update value in db");
}

mongocxx::cursor MongoDb::get_fields(const string& collect_str, vector<string>& fields) const
{
    collection collect = database_[collect_str];
    mongocxx::options::find opts {};
    bson_doc_bld doc;
    for (const auto& field : fields)
        doc << field << 1;
    doc << finalize;
    opts.projection(doc.view());
    cursor result = collect.find(bson_doc_bld {} << finalize, opts);
    return result;
}

mongocxx::cursor MongoDb::get_fields(const string& collect_str, vector<string>& fields, const bson_doc_bld& filter) const
{
    collection collect = database_[collect_str];
    mongocxx::options::find opts {};
    bson_doc_bld doc;
    for (const auto& field : fields)
        doc << field << 1;
    doc << finalize;
    opts.projection(doc.view());
    cursor result = collect.find(filter.view(), opts);
    return result;
}

uint64_t MongoDb::get_docs_no(const string& collect_str) const
{
    collection collect = database_["collect_str"];
    return collect.count_documents({});
}
