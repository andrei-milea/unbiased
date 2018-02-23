#define BOOST_TEST_MODULE "test_article_builder"

#include "../article_builder.h"
#include "../config.h"
#include "boost/test/included/unit_test.hpp"
#include <streambuf>
#include <string>
#include <fstream>
#include <vector>
#include <boost/algorithm/string.hpp>

using namespace boost::unit_test;
using namespace boost;
using namespace std;

vector<string> load_articles_xml(const string& articles_filename)
{
	std::ifstream articles_file(articles_filename);
	if(!articles_file.is_open())
		throw std::runtime_error("load_articles_xml error: failed to open articles file - " + articles_filename);

	std::string articles_str((istreambuf_iterator<char>(articles_file)), istreambuf_iterator<char>());
	typedef split_iterator<string::iterator> string_split_iterator;
	std::vector<std::string> articles_xml;
    for(auto it = make_split_iterator(articles_str, first_finder("</article>", is_iequal())); it != string_split_iterator(); ++it)
    {
		articles_xml.push_back(copy_range<std::string>(*it) + "</article>");
    }
	return articles_xml;
}

BOOST_AUTO_TEST_CASE(test_article_from_xml)
{
	std::vector<std::pair<std::string, Signature>> docs_signatures;
	ArticleBuilder article_builder(0, docs_signatures);
	auto articles_xml = load_articles_xml("articles.xml");
	for(const auto& article_xml : articles_xml)
	{
		article_builder.from_xml(article_xml);
	}
}

