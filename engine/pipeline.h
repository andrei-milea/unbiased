#ifndef _PIPELINE_H
#define _PIPELINE_H

#include "utils/log_helper.h"
#include "article.h"
#include "vocabulary.h"
#include "mongodb.h"
#include "bu_clustering.h"
#include "article_dedup.h"
#include <zmq.hpp>
#include <boost/asio.hpp>
#include <boost/process.hpp>
#include <string>
#include <thread>

class Pipeline
{
public:
    explicit Pipeline(size_t buff_max_size);

    ~Pipeline()
    {
        stop_processing_queue();
    }

    //compute term-doc matrix and clusters to initialize the pipeline for further processing
    //void process_batch(const std::vector<Article> &articles);
    void process_batch(const std::string& filename_path);


    //start asio service and processing threads
    void start_processing_queue(uint32_t threads_no);

    //stop asio service and threads
    void stop_processing_queue();

    //enqueues article url to be processed
    void enqueue_article(const std::string& url);

private:
    //parse scrapped article, run dedup and clustering
    void process_article(const std::string& article_str) noexcept;

    void sample_batch()const;

    bool is_duplicate(const std::pair<std::string, Signature> &id_sign) const;

private:
    boost::asio::io_service asio_service_;
    size_t buffer_max_size_;
    mutable std::atomic<int32_t> processed_articles_;
    Vocabulary vocabulary_;
    ArticleParser article_parser_;
    MinHash minhash_;
    std::unique_ptr<Clustering> clustering_processor_;
    mutable zmq::socket_t zmq_client_sock_;

    //mongodb dastase access point - one connection per thread
    static thread_local MongoDb mongodb_inst_;

public:
    static zmq::context_t zmq_context_;

};

#endif
