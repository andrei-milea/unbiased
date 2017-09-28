#ifndef CONFIG_H
#define CONFIG_H

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <iostream>

using boost::property_tree::ptree;

/// singleton used to load the configuration file
class Config
{
public:
	size_t threads_no;
	size_t scrapper_buff_size;
	std::string host;
	std::string port;
	std::string username;
	std::string password;
	std::string dbname; 

    static const Config& get()
    {
        static Config cfg("config.xml");
        return cfg;
    }

private:
    Config(){}
    explicit Config(const std::string &cfg_file)
    {
        try
        {
            ptree pt;
            read_xml(cfg_file, pt);
            threads_no = pt.get<size_t>("config.threads_no");
			scrapper_buff_size = pt.get<size_t>("config.scrapper_buff_size");
            host = pt.get<std::string>("config.postgresql_db.host");
            port = pt.get<std::string>("config.postgresql_db.port");
            dbname = pt.get<std::string>("config.postgresql_db.dbname");
            username = pt.get<std::string>("config.postgresql_db.username");
            password = pt.get<std::string>("config.postgresql_db.password");
        }
        catch(std::exception& ex)
        {
            std::cout << "error parsing the config file: " << ex.what() << std::endl;
            exit(1);
        }
    }
};




#endif

