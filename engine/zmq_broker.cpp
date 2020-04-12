
//#include "zhelpers.hpp"
#include <zmq.hpp>
#include <iostream>

using namespace std;

int main (int argc, char *argv[])
{
    try
    {
        zmq::context_t context(1);

        //  Socket facing clients
        zmq::socket_t frontend (context, ZMQ_PULL);
        frontend.bind("ipc:///tmp/zmq_frontend_receiver");

        //  Socket facing services
        zmq::socket_t backend (context, ZMQ_PUSH);
        backend.bind("ipc:///tmp/zmq_backend_sender");

        //  Start the proxy
        zmq::proxy(frontend, backend);
    }
    catch (const std::exception& ex)
    {
        cout << "zmq_worker critical error: " << ex.what() << "\n";
    }
    return 0;
}
