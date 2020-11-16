#include "pipeline.h"
#include "article_utils.h"
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

/*void Pipeline::process_batch(const std::vector<Article> &articles)
{
    processed_articles_ += articles.size();
    clustering_processor_->create_clusters(articles);
}*/

void Pipeline::process_batch(const string& filename_path)
{
    vector<pair<string, Signature>> docs_signatures;
    vector<Article> articles;
    auto articles_xml = load_articles_xml(filename_path);
    for(const auto& article_xml : articles_xml)
    {
        Article new_article;
        article_parser_.parse_from_xml(article_xml, new_article);

        set<string> stems;
        bool valid = article_parser_.tokenize_validate(new_article, stems);
        if (valid)
        {
            vocabulary_.add_stems(std::move(stems));
            articles.push_back(std::move(new_article));
        }
        else
        {
            save_invalid_url(mongodb_inst_, new_article.meta_data.url);
            spdlog::info("invalid article url - {}, title - {}", new_article.meta_data.url, new_article.meta_data.title);
            continue;
        }
    }

    spdlog::info("articles: {} valid articles: {}", articles_xml.size(), articles.size());
    processed_articles_ += articles.size();
    std::vector<ProcessedArticle> processed_articles;
    for (size_t idx = 0; idx < articles.size(); idx++)
    {
        ProcessedArticle proc_article;
        auto shingles = article_parser_.process_tokens<set<int32_t>>(articles[idx], proc_article);

        string id = save_article(Pipeline::mongodb_inst_, proc_article);
        proc_article.signature = minhash_.compute_signature(shingles);
        if (!is_duplicate(make_pair(id, proc_article.signature)))
            processed_articles.push_back(std::move(proc_article));
    }

    clustering_processor_->create_clusters(processed_articles);
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
            spdlog::info("Pipeline::enqueue_article error processing article - {}", ec.message());
    };
    //boost::asio::async_read(apipe, boost::asio::buffer(&buffer_str[0], buffer_str.size()), get_res);
}

void Pipeline::process_article(const string& article_str) noexcept
{
    Article new_article;
    try
    {
        article_parser_.parse_from_xml(article_str, new_article);
        set<string> stems;
        bool valid = article_parser_.tokenize_validate(new_article, stems);
        if (valid)
        {
            vocabulary_.add_stems(std::move(stems));
            ProcessedArticle proc_article;
            auto shingles = article_parser_.process_tokens<set<int32_t>>(new_article, proc_article);

            string id = save_article(Pipeline::mongodb_inst_, proc_article);
            proc_article.signature = minhash_.compute_signature(shingles);
            if (is_duplicate(make_pair(id, proc_article.signature)))
                return;

            clustering_processor_->add_to_clusters(proc_article);
            processed_articles_++;
        }
        else
        {
            save_invalid_url(mongodb_inst_, new_article.meta_data.url);
            spdlog::info("invalid article url - {}, title - {}", new_article.meta_data.url, new_article.meta_data.title);
        }
    }
    catch (const std::exception& ex)
    {
        spdlog::warn("exception {} while processing article url - {}, title - {}",
            ex.what(), new_article.meta_data.url, new_article.meta_data.title);
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
