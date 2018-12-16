#include "engine.h"
#include "pqldb.h"
#include "mongodb.h"
#include "lsa.h"
#include "utils/article_utils.h"
#include <csignal>
#include <exception>

#include <qpid/messaging/Connection.h>
#include <qpid/messaging/Message.h>
#include <qpid/messaging/Receiver.h>
#include <qpid/messaging/Sender.h>
#include <qpid/messaging/Session.h>

using namespace std;
using namespace qpid::messaging;

/*void init()
{
	//compute term doc matrix
	//with available data
	const string articles_path("articles.xml");
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
	lsa_processor.run_svd();
}*/

int main()
{
	Engine engine;
	engine.get_processor().run(3);

	try
	{
		const string broker = "localhost:5673";
		const string address = "unbiased.new_urls; {create: always, node: {type: 'queue'}}";
		const string connection_options = "";
		Connection connection(broker, connection_options);
		connection.open();
		Session session = connection.createSession();
		Receiver receiver = session.createReceiver(address);
		while(true)
		{
			auto message = receiver.fetch();
			std::cout << message.getContent() << std::endl;
			//engine.get_processor().scrap_and_process(message.getContent());
			session.acknowledge();
		}
	}
	catch(const std::exception& ex)
	{
		cout << "critical error" << ex.what() << "\n";
	}
	return 0;
}
