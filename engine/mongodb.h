#ifndef _MONGODB_H
#define _MONGODB_H

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
	

	std::vector<string> load_vocabulary()const
	{

	}

	std::vector<Article> load_articles()const
	{
		std::vector<Article> articles;
	}

private:
	MongoDb()
		db_str_(Config::get().mongo_credentials.dbname),
		client_conn_(get_connection_uri())
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

