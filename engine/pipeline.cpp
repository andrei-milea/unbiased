#include "pipeline.h"
#include "utils/article_utils.h"
#include "mongodb_helper.h"
#include "article_parser.h"
#include "zmq_helper.h"
#include <exception>

using namespace std;
namespace bp = boost::process;

thread_local MongoDb Pipeline::mongodb_inst_{Config::get().mongo_credentials.dbname};
zmq::context_t Pipeline::zmq_context_{1};

Pipeline::Pipeline(size_t buff_max_size)
    : buffer_max_size_(buff_max_size)
    , processed_articles_{0}
    , article_parser_{vocabulary_}
    , minhash_{SIGNATURE_SIZE, MINHASH_RANGE_MAX}
    , clustering_processor_{make_unique<BU_Clustering>(vocabulary_)}
    , zmq_client_sock_{Pipeline::zmq_context_, ZMQ_REQ}
{
    zmq_client_sock_.connect("ipc://server.ipc");
    load_vocabulary(Pipeline::mongodb_inst_, vocabulary_);
}

void Pipeline::process_batch(const std::vector<Article> &articles)
{
    processed_articles_ += articles.size();
    clustering_processor_->create_clusters(articles);
}

void Pipeline::process_batch(const string& filename_path)
{
    vector<pair<string, Signature>> docs_signatures;
    vector<Article> articles;
    auto articles_xml = load_articles_xml(filename_path);
    for(const auto& article_xml : articles_xml)
    {
        Article new_article;
        article_parser_.parse_from_xml(article_xml, new_article);

        if(!new_article.is_valid(0.17))
        {
            save_invalid_url(mongodb_inst_, new_article.url);
            spdlog::info("invalid article url - {}, title - {}", new_article.url, new_article.title);
            continue;
        }

        auto shingles = article_parser_.process_tokens<set<int32_t>>(new_article);
        new_article.signature = minhash_.compute_signature(shingles);
        string id = save_article(Pipeline::mongodb_inst_, new_article);
        if(!is_duplicate(make_pair(id, new_article.signature)))
            articles.push_back(new_article);
    }

    processed_articles_ += articles.size();
    spdlog::info("articles:  {}, valid articles: {}", articles_xml.size(), articles.size());
    clustering_processor_->create_clusters(articles);
}

void Pipeline::start_processing_queue(uint32_t threads_no)
{
    for (size_t idx = 0; idx < threads_no; idx++)
    {
        thread new_thread([&] { asio_service_.run(); });
        new_thread.detach();
    }
}

void Pipeline::stop_processing_queue()
{
    spdlog::info("stopping asio processing service");
    asio_service_.stop();
}

void Pipeline::enqueue_article(const std::string& url)
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
}

void Pipeline::process_article(const string& article_str) noexcept
{
    Article article;
    try
    {
        article_parser_.parse_from_xml(article_str, article);
        if(!article.is_valid(0.17))
        {
            save_invalid_url(Pipeline::mongodb_inst_, article.url);
            spdlog::info("invalid article url - {}, title - {}", article.url, article.title);
            return;
        }
        auto shingles = article_parser_.process_tokens<set<int32_t>>(article);
        article.signature = minhash_.compute_signature(shingles);
        string id = save_article(Pipeline::mongodb_inst_, article);
        if(!is_duplicate(make_pair(id, article.signature)))
        {
            clustering_processor_->add_to_clusters(article);
            processed_articles_++;
        }
    }
    catch (const std::exception& ex)
    {
        spdlog::warn("exception {} while processing article url - {}, title - {}",
                ex.what(), article.url, article.title);
    }
}

bool Pipeline::is_duplicate(const pair<string, Signature> &id_sign)const
{
    string msg = serialize_id_sign(id_sign);
    s_send(zmq_client_sock_, msg);
    string res = s_recv(zmq_client_sock_);
    if (res == "DUP")
        return true;
    assert(res == "OK");
    return false;
}

void Pipeline::sample_batch()const
{
}
