#ifndef _DB_H
#define _DB_H

#include <pqxx>
#include <string>

class DbAccess
{
public:
	DbAccess(const std::string& params)
		:pqdb_connection_(params.c_str())
	{
	}

	std::vector<std::vector<std::string>> execute(const std::string& query)
	{
		pqxx::work work_trans(pqdb_connection_);
		pqxx::result res = work_trans.exec(query);
		work_trans.commit();

		std::vector<std::vector<std::string>> res_str(res.size());
		for(size_t row_idx = 0; row_idx < res.size(); row_idx++)
			for(size_t col_idx = 0; col_idx < res[row_idx].size(); col_idx++)
				res_str[row_idx].emplace_back(res[row_idx][col_idx].as<std::string>());
		return res_str;
	}

private:
	pqxx::connection pqdb_connection_;

};

#endif

