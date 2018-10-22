#define BOOST_TEST_MODULE "test_lsa"

#include "../mongodb.h"
#include "article_utils.h"
#include "../lsa.h"
#include "../config.h"
#include "boost/test/included/unit_test.hpp"

using namespace boost::unit_test;
using namespace boost;
using namespace std;

BOOST_AUTO_TEST_CASE(test_lsa_svd)
{
	std::vector<std::pair<std::string, Signature>> docs_signatures;
	ArticleBuilder article_builder(docs_signatures, 0.2);
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

	//save articles to mongodb
	auto& mongo_inst = MongoDb::get("test_db");
	mongo_inst.drop_collection("articles");
	for(const auto& article : articles)
		mongo_inst.save_article(article);

	auto articles_no = mongo_inst.get_articles_no();
	BOOST_REQUIRE_EQUAL(articles_no, articles.size());

	cout << "articles: " << articles_xml.size() << " valid articles: " << articles.size() << "\n";
	size_t all_articles_no = mongo_inst.get_articles_no();
	LSA lsa_processor(article_builder.get_vocabulary());
	lsa_processor.build_term_doc_matrix(articles, all_articles_no);
	lsa_processor.run_svd();
}
