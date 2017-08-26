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
	std::string host;
	std::string port;
	std::string username;
	std::string password;
	std::string dbname; 

    static const Config& get()
    {
        static Config cfg("params.xml");
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
            host = pt.get<std::string>("config.posqresql_db.host");
            port = pt.get<std::string>("config.posqresql_db.port");
            dbname = pt.get<std::string>("config.posqresql_db.dbname");
            username = pt.get<std::string>("config.posqresql_db.username");
            password = pt.get<std::string>("config.posqresql_db.password");
        }
        catch(std::exception& ex)
        {
            std::cout << "error parsing the config file: " << ex.what() << std::endl;
            exit(1);
        }
    }
};




#endif

