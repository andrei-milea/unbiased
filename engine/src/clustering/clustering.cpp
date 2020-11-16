#include "../../include/clustering/pipeline.h"
#include "config.h"
#include "log_helper.h"

#include <exception>
#include <zmq.hpp>
#include <string>

using namespace std;

int main()
{
    try
    {
        spdlog::set_default_logger(std::make_shared<spdlog::logger>("runtime_logger", std::make_shared<spdlog::sinks::daily_file_sink_mt>("clustering.log", 23, 59)));
        Pipeline pipeline{Config::get().scrapper_buff_size};
        pipeline.process_batch("articles.xml");
        pipeline.start_processing_queue(3);

        zmq::socket_t receiver{Pipeline::zmq_context_, ZMQ_PULL};
        receiver.connect("ipc:///tmp/zmq_backend_sender");

        //process all received urls
        while (true)
        {
            zmq::message_t message;
            receiver.recv(message);
            std::string message_str(static_cast<char*>(message.data()), message.size());
	        pipeline.enqueue_article(message_str);
        }
    }
    catch (const std::exception& ex)
    {
        spdlog::error("quiting, clustering exception: {}", ex.what());
    }
    return 0;
}
