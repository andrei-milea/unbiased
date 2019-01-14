#define BOOST_TEST_MODULE "test_lsa"

#include "../mongodb.h"
#include "../utils/article_utils.h"
#include "../lsa.h"
#include "../config.h"
#include "boost/test/included/unit_test.hpp"

using namespace boost::unit_test;
using namespace boost;
using namespace std;

BOOST_AUTO_TEST_CASE(test_lsa_svd)
{
	std::vector<std::pair<std::string, Signature>> docs_signatures;
	ArticleBuilder article_builder{/*0.17*/0.3};
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
		//else
			//cout << "title: " << new_article.title << endl;
	}

	cout << "articles: " << articles_xml.size() << " valid articles: " << articles.size() << "\n";
	LSA lsa_processor{article_builder.get_vocabulary()};
	lsa_processor.build_term_doc_matrix(articles, articles.size());
	lsa_processor.run_svd();
	//lsa_processor.print_term_doc_matrix();
	lsa_processor.print_top_concepts(0.09);
}

