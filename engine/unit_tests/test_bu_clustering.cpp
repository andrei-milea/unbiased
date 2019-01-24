#define BOOST_TEST_MODULE "test_bu_clustering"

#include "../utils/article_utils.h"
#include "../bu_clustering.h"
#include "boost/test/included/unit_test.hpp"

using namespace boost::unit_test;
using namespace boost;
using namespace std;

BOOST_AUTO_TEST_CASE(test_clustering)
{
	ArticleBuilder article_builder{0.3};
	vector<Article> articles;
	auto articles_xml = load_articles_xml("articles.xml");
	for(const auto& article_xml : articles_xml)
	{
		Article new_article;
		auto result = article_builder.from_xml(article_xml, new_article);
		if(result == BuilderRes::VALID || result == BuilderRes::DUPLICATE)
		{
			articles.push_back(new_article);
		}
	}

	cout << "articles: " << articles_xml.size() << " valid articles: " << articles.size() << "\n";

	BU_Clustering clust_proc{article_builder.get_vocabulary()};
	clust_proc.create_clusters(articles);
}

