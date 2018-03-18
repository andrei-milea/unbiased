#ifndef _ARTICLE_UTILS_H
#define _ARTICLE_UTILS_H

#include "../article_builder.h"
#include <streambuf>
#include <string>
#include <fstream>
#include <vector>
#include <boost/algorithm/string.hpp>

using namespace boost;
using namespace std;

inline vector<string> load_articles_xml(const string& articles_filename)
{
	std::ifstream articles_file(articles_filename);
	if(!articles_file.is_open())
		throw std::runtime_error("load_articles_xml error: failed to open articles file - " + articles_filename);

	std::string articles_str((istreambuf_iterator<char>(articles_file)), istreambuf_iterator<char>());
	typedef split_iterator<string::iterator> string_split_iterator;
	std::vector<std::string> articles_xml;
    for(auto it = make_split_iterator(articles_str, first_finder("</article>\n\n", is_iequal())); it != string_split_iterator(); ++it)
    {
		if(!copy_range<std::string>(*it).empty())
			articles_xml.push_back(copy_range<std::string>(*it) + "</article>");
    }
	return articles_xml;
}


#endif

