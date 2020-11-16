#ifndef CONFIG_H
#define CONFIG_H

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <iostream>

using boost::property_tree::ptree;

constexpr int32_t SIGNATURE_SIZE = 100;
using Signature = std::array<int32_t, SIGNATURE_SIZE>;

struct PQLCredentials
{
    std::string host;
    std::string port;
    std::string username;
    std::string password;
    std::string dbname;
};

struct MongoCredentials
{
    std::string server;
    std::string username;
    std::string password;
    std::string dbname;
};

/// singleton used to load the configuration file
class Config
{
public:
    size_t threads_no;
    size_t scrapper_buff_size;
    size_t update_vocab_freq;
    std::string vocabulary_path;
    PQLCredentials pql_credentials;
    MongoCredentials mongo_credentials;

    static const Config& get()
    {
        static Config cfg { "config.xml" };
        return cfg;
    }

    Config(const Config&) = delete;
    Config(Config&&) = delete;
    Config& operator=(const Config&) = delete;
    Config& operator=(Config&&) = delete;

private:
    Config() {}
    explicit Config(const std::string& cfg_file)
    {
        try
        {
            ptree pt;
            read_xml(cfg_file, pt);
            threads_no = pt.get<size_t>("config.threads_no");
            scrapper_buff_size = pt.get<size_t>("config.scrapper_buff_size");
            update_vocab_freq = pt.get<size_t>("config.update_vocab_freq");
            vocabulary_path = pt.get<std::string>("config.vocabulary_path");

            pql_credentials.host = pt.get<std::string>("config.postgresql.host");
            pql_credentials.port = pt.get<std::string>("config.postgresql.port");
            pql_credentials.username = pt.get<std::string>("config.postgresql.username");
            pql_credentials.password = pt.get<std::string>("config.postgresql.password");
            pql_credentials.dbname = pt.get<std::string>("config.postgresql.dbname");

            mongo_credentials.server = pt.get<std::string>("config.mongodb.server");
            mongo_credentials.username = pt.get<std::string>("config.mongodb.username");
            mongo_credentials.password = pt.get<std::string>("config.mongodb.password");
            mongo_credentials.dbname = pt.get<std::string>("config.mongodb.dbname");
        }
        catch (std::exception& ex)
        {
            std::cout << "error parsing the config file: " << ex.what() << std::endl;
            exit(1);
        }
    }
};

#endif
