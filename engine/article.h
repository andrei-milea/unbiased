#ifndef _ARTICLE_H
#define _ARTICLE_H

#include <string>
#include <vector>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>

using boost::property_tree::ptree;

class Article
{
public:
	explicit Article(const std::string &article_str)
    {
		ptree pt;
		read_xml(article_str, pt);
		title_ = pt.get<std::string>("article.title");
		date_ = pt.get<std::string>("article.date");
		auto keywords_node = pt.get_child("article.keywords");
		for (const auto& keyword_node : keywords_node)
		{
			keywords_.push_back(keyword_node.second.get<std::string>("keyword"));
		}
	}

private:
	std::string title_;
	std::string date_;
	std::vector<std::string> keywords_;
};

#endif

