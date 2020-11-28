#ifndef _ZMQ_HELPER_H
#define _ZMQ_HELPER_H

#include <boost/serialization/string.hpp>
#include <boost/serialization/utility.hpp>
#include <boost/serialization/array.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <zmq.hpp>
#include <sstream>
#include <utility> 
#include "config.h"

//  Receive 0MQ string from socket and convert into string
inline std::string s_recv (zmq::socket_t & socket)
{
    zmq::message_t message;
    socket.recv(message);
    return std::string(static_cast<char*>(message.data()), message.size());
}

//  Convert string to 0MQ string and send to socket
inline bool s_send (zmq::socket_t & socket, const std::string & string)
{
    zmq::message_t message(string.size());
    memcpy(message.data(), string.data(), string.size());
    auto rc = socket.send(message, zmq::send_flags::none);
    return (*rc);
}

//  Sends string as 0MQ string, as multipart non-terminal
inline bool s_sendmore (zmq::socket_t & socket, const std::string & string)
{
    zmq::message_t message(string.size());
    memcpy(message.data(), string.data(), string.size());
    auto rc = socket.send(message, zmq::send_flags::sndmore);
    return (*rc);
}

#endif
