#include "engine.h"
#include "pqldb.h"
#include "mongodb.h"
#include "lsa.h"
#include "utils/article_utils.h"
#include <csignal>
#include <exception>

using namespace std;
Engine g_engine;

void signal_handler(int sig)
{
	cout << "handler called\n";
	const std::string select_entries_str{"SELECT * FROM backend_userentry WHERE processed = False"};
	try
	{
		auto user_entries = PqlDb::get().get_unprocessed_entries();
		for(const auto& user_entry : user_entries)
		{
			assert(user_entry.size() == 5);
			g_engine.get_processor().scrap_and_process(user_entry[1]);
		}
		//thread_pool.post();
		
		/*for(const auto& user_entry : user_entries)
		{
			cout << "entry: ";
			for(const auto& entry_elem : user_entry)
				cout << entry_elem << " ";
			cout << endl;
		}*/
	}
	catch(const std::exception& ex)
	{
		cout << "signal_handler failed: " << ex.what() << endl;
	}
}

int main()
{
	//compute term doc matrix
	//with available data
	/*const string articles_path("articles.xml");
	std::vector<std::pair<std::string, Signature>> docs_signatures;
	ArticleBuilder article_builder(docs_signatures, 0.17);
	vector<Article> articles;
	auto articles_xml = load_articles_xml(articles_path);
	for(const auto& article_xml : articles_xml)
	{
		Article new_article;
		auto result = article_builder.from_xml(article_xml, new_article);
		if(result == BuilderRes::VALID || result == BuilderRes::DUPLICATE)
		{
			articles.push_back(new_article);
		}
	}

	//save articles to mongodb
	auto& mongo_inst = MongoDb::get("unbiased");
	mongo_inst.drop_collection("articles");
	for(const auto& article : articles)
		mongo_inst.save_article(article);

	auto articles_no = mongo_inst.get_articles_no();
	assert(articles_no == articles.size());

	cout << "articles: " << articles_xml.size() << " valid articles: " << articles.size() << "\n";
	size_t all_articles_no = mongo_inst.get_articles_no();
	LSA lsa_processor(article_builder.get_vocabulary());
	lsa_processor.build_term_doc_matrix(articles, all_articles_no);
	lsa_processor.run_svd();*/

	//register signal from django
	signal(SIGUSR1, signal_handler);	
	signal_handler(SIGUSR1);
	while(true)
	{
	}
	return 0;
}
