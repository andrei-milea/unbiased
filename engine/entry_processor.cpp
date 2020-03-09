#include "entry_processor.h"
#include "bu_clustering.h"

using namespace std;
namespace bp = boost::process;

EntryProcessor::EntryProcessor(size_t buff_max_size)
    : buffer_max_size_(buff_max_size)
    , processed_articles_(0)
    , cluster_proc_(make_unique<BU_Clustering>(article_builder_.get_vocabulary()))
{
}

void EntryProcessor::init()
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
			articles.push_back(new_article);
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
}

void EntryProcessor::run(uint32_t threads_no)
{
    for (size_t idx = 0; idx < threads_no; idx++)
    {
        thread new_thread([&] { asio_service_.run(); });
        new_thread.detach();
    }
}

//enqueues process_article
void EntryProcessor::scrap_and_process(const std::string& url)
{
    string buffer_str('\0', buffer_max_size_);
    bp::async_pipe apipe(asio_service_);
    bp::child c(bp::search_path("python"), std::string("scrap.py ") + url, bp::std_out > apipe);

    auto get_res = [this, &buffer_str](const boost::system::error_code& ec, std::size_t size) {
        if (!ec)
            process_article(buffer_str);
        else
            std::cout << ec.message() << std::endl;
    };
    boost::asio::async_read(apipe, boost::asio::buffer(&buffer_str[0], buffer_str.size()), get_res);
    if (processed_articles_ > Config::get().update_vocab_freq)
    {
        //wait for all articles to be processed
        processed_articles_ = 0;
        article_builder_.update_vocab_freq();
    }
}

void EntryProcessor::process_article(const string& entry_str) noexcept
{
    try
    {
        Article article;
        BuilderRes result = article_builder_.from_xml(entry_str, article);
        if (result == BuilderRes::VALID || result == BuilderRes::DUPLICATE)
        {
            article_builder_.save_article(article);
            processed_articles_++;
        }
    }
    catch (exception& ex)
    {
        cout << "error processing article: " << ex.what() << endl;
    }
}
