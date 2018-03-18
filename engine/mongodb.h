#ifndef _MONGODB_H
#define _MONGODB_H

#include "vocabulary.h"
#include "article.h"
#include "config.h"

#include <curl/curl.h>
#include <unordered_set>
#include <map>
#include <vector>
#include <string>
#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#include <bsoncxx/builder/stream/document.hpp>

class MongoDb
{
public:
	static MongoDb& get(const std::string& dbname = std::string())
	{
		static MongoDb instance(dbname);
		return instance;
	}

	void save_article(const Article& article);

	void drop_collection(const std::string& dbname)
	{
		database_[dbname].drop();
	}

	std::vector<Article> load_articles(const std::string& key, const std::string& value)const;

	std::vector<std::pair<std::string, Signature>> load_articles_signatures()const;

	std::vector<std::pair<std::string, std::string>> load_articles_dates(const std::vector<std::string> &articles_ids)const;

	uint64_t get_articles_no()const;

private:
	MongoDb(const std::string& db_name)
		:db_str_(db_name.empty() ? Config::get().mongo_credentials.dbname : db_name),
		client_conn_(get_connection_uri()),
		database_(client_conn_[db_str_])
	{
	}

	mongocxx::uri get_connection_uri()const
	{
		return mongocxx::uri{"mongodb://" + Config::get().mongo_credentials.username  + ":" + url_encode(Config::get().mongo_credentials.password) + "@" + Config::get().mongo_credentials.server};
	}

	MongoDb(const MongoDb&) = delete;
	MongoDb(MongoDb&&) = delete;
	MongoDb& operator=(const MongoDb&) = delete;
	MongoDb& operator=(MongoDb &&) = delete;

	std::string url_encode(const std::string &source)const
	{
		CURL *curl = curl_easy_init();
		char *cres = curl_easy_escape(curl, source.c_str(), source.length());
		std::string res(cres);
		curl_free(cres);
		curl_easy_cleanup(curl);
		return res;
	}

private:
	std::string db_str_;
	mongocxx::instance inst_;
	mongocxx::client client_conn_;
	mongocxx::database database_;
	bsoncxx::builder::stream::document bson_builder_;
};

#endif

