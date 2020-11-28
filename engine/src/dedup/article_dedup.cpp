#include "article_dedup.h"
#include "log_helper.h"
#include "dedup_rpc_stub.h"
#include <thread>
#include <zmq.hpp>

using namespace std;

int main()
{
    spdlog::set_default_logger(std::make_shared<spdlog::logger>("runtime_logger", std::make_shared<spdlog::sinks::daily_file_sink_mt>("dedup.log", 23, 59)));
    try
    {
        ArticleDedup article_dedup;
        zmq::context_t context(1);
        zmq::socket_t zmq_frontend(context, ZMQ_ROUTER);
        //communicate externally with clients
        zmq_frontend.bind("ipc://dedup_server.ipc");
        while (true)
        {
            std::string identity = s_recv(zmq_frontend);
            s_recv(zmq_frontend);//envelope delimiter
            std::string id_sign_str = s_recv(zmq_frontend);//signature and id

            auto id_sign = deserialize_id_sign(id_sign_str);

            bool res = article_dedup.is_duplicate(id_sign);
            //spdlog::info("article_dedup result: {}", res);
            string res_str{ res ? "DUP" : "OK" };

            s_sendmore(zmq_frontend, identity);
            s_sendmore(zmq_frontend, "");
            s_send(zmq_frontend, res_str);
        }
    }
    catch (const std::exception& ex)
    {
        spdlog::error("article_dedup exception: {}", ex.what());
    }

    return 0;
}
