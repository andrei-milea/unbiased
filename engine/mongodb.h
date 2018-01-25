#ifndef _MONGODB_H
#define _MONGODB_H

#include "vocabulary.h"
#include "article.h"
#include "config.h"

#include <unordered_set>
#include <map>
#include <vector>
#include <string>
#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>



class MongoDb
{
public:
	static MongoDb& get()
	{
		static MongoDb instance;
		return instance;
	}
	

	std::map<std::string, WordInfo> load_vocabulary_map()const;

	std::unordered_set<std::string> load_vocabulary_stop_words()const;

	std::vector<Article> load_articles()const;

	uint64_t get_articles_no()const;

private:
	MongoDb()
		:db_str_(Config::get().mongo_credentials.dbname),
		client_conn_(get_connection_uri()),
		database_(client_conn_[db_str_])
	{
	}

	mongocxx::uri get_connection_uri()const
	{
		return mongocxx::uri{"mongodb://" + Config::get().mongo_credentials.username  + ":" + Config::get().mongo_credentials.password + "@" + Config::get().mongo_credentials.server};
	}

	MongoDb(const MongoDb&) = delete;
	MongoDb(MongoDb&&) = delete;
	MongoDb& operator=(const MongoDb&) = delete;
	MongoDb& operator=(MongoDb &&) = delete;

private:
	std::string db_str_;
	mongocxx::instance inst_;
	mongocxx::client client_conn_;
	mongocxx::database database_;
};

#endif

