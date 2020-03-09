#ifndef _MONGODB_H
#define _MONGODB_H

#include "config.h"
#include <bsoncxx/builder/stream/document.hpp>
#include <curl/curl.h>
#include <map>
#include <mongocxx/client.hpp>
#include <mongocxx/cursor.hpp>
#include <mongocxx/instance.hpp>
#include <string>
#include <unordered_set>
#include <vector>

using bson_doc_bld = bsoncxx::builder::stream::document;
using bson_doc = bsoncxx::document::value;

class MongoDb
{
public:
    MongoDb(const std::string& db_name = "")
        : db_str_(db_name.empty() ? Config::get().mongo_credentials.dbname : db_name)
        , client_conn_(get_connection_uri())
        , database_(client_conn_[db_str_])
    {
    }

    void save_doc(const std::string& collect_str, bson_doc doc);

    void update_doc(const std::string& collect_str, const bson_doc& doc,
        const std::string& key = "", const std::string& value = "");

    void drop_collection(const std::string& collect_str)
    {
        database_[collect_str].drop();
    }

    mongocxx::cursor get_docs(const std::string& collect_str, const std::string& key = "",
        const std::string& value = "") const;

    mongocxx::cursor get_fields(const std::string& collect_str, std::vector<std::string>& fields) const;

    mongocxx::cursor get_fields(const std::string& collect_str, std::vector<std::string>& fields,
        const bson_doc_bld& filter) const;

    uint64_t get_docs_no(const std::string& collect_str) const;

    MongoDb(const MongoDb&) = delete;
    MongoDb(MongoDb&&) = delete;
    MongoDb& operator=(const MongoDb&) = delete;
    MongoDb& operator=(MongoDb&&) = delete;

private:
    mongocxx::uri get_connection_uri() const
    {
        return mongocxx::uri { "mongodb://" + Config::get().mongo_credentials.username + ":" + url_encode(Config::get().mongo_credentials.password) + "@" + Config::get().mongo_credentials.server };
    }

    std::string url_encode(const std::string& source) const
    {
        CURL* curl = curl_easy_init();
        char* cres = curl_easy_escape(curl, source.c_str(), source.length());
        std::string res { cres };
        curl_free(cres);
        curl_easy_cleanup(curl);
        return res;
    }

private:
    std::string db_str_;
    mongocxx::instance inst_;
    mongocxx::client client_conn_;
    mongocxx::database database_;
};

#endif
