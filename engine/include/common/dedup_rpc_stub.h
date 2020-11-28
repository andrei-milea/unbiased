#ifndef _DEDUP_RPC_STUB_H
#define _DEDUP_RPC_STUB_H

#include "zmq_helper.h"

//serialize id, signature to string
inline std::string serialize_id_sign(const std::pair<std::string, Signature> &id_sign)
{
    std::stringstream ss;
    boost::archive::text_oarchive oa{ss};
    oa << id_sign;
    return ss.str();
}

//deserialize id, signature from string
inline std::pair<std::string, Signature> deserialize_id_sign(const std::string &str)
{
    std::pair<std::string, Signature> res;
    std::stringstream ss{str};
    boost::archive::text_iarchive ia{ss};
    ia >> res;
    return res;
}

class DedupRPCStub
{
public:
    DedupRPCStub(zmq::context_t& ctx)
        : zmq_client_sock_{ctx, ZMQ_REQ}
    {
        zmq_client_sock_.connect("ipc://dedup_server.ipc");
    }

    bool is_duplicate(const std::pair<std::string, Signature> &id_sign)const
    {
        std::string msg = serialize_id_sign(id_sign);
        s_send(zmq_client_sock_, msg);
        std::string res = s_recv(zmq_client_sock_);
        if (res == "DUP")
            return true;
        assert(res == "OK");
        return false;
    }

private:
    mutable zmq::socket_t zmq_client_sock_;
};

#endif
