#ifndef _PQLDB_H
#define _PQLDB_H

#include "config.h"
#include </usr/include/pqxx/pqxx>
#include <string>

class PqlDb
{
public:
    PqlDb()
        : pqdb_connection_(get_connection_params())
    {
    }

    PqlDb(const PqlDb&) = delete;
    PqlDb(PqlDb&&) = delete;
    PqlDb& operator=(const PqlDb&) = delete;
    PqlDb& operator=(PqlDb&&) = delete;

    std::vector<std::vector<std::string>> get_unprocessed_entries()
    {
        const std::string select_entries_str { "SELECT * FROM backend_userentry WHERE processed = False" };
        return execute(select_entries_str);
    }

    std::vector<std::vector<std::string>> execute(const std::string& query)
    {
        pqxx::work work_trans { pqdb_connection_ };
        pqxx::result res = work_trans.exec(query);
        work_trans.commit();

        std::vector<std::vector<std::string>> res_str { res.size() };
        for (size_t row_idx = 0; row_idx < res.size(); row_idx++)
            for (size_t col_idx = 0; col_idx < res[(int)row_idx].size(); col_idx++)
                res_str[row_idx].emplace_back(res[(int)row_idx][(int)col_idx].as<std::string>());
        return res_str;
    }

private:
    std::string get_connection_params() const
    {
        return std::string { "host=" } + Config::get().pql_credentials.host + " user=" + Config::get().pql_credentials.username + " password=" + Config::get().pql_credentials.password + " dbname=" + Config::get().pql_credentials.dbname;
    }

private:
    pqxx::connection pqdb_connection_;
};

#endif
