#ifndef _STEMID_TOKENS_RCUMAP_H
#define _STEMID_TOKENS_RCUMAP_H

#include <array>
#include <atomic>
#include <memory>
#include <boost/container/flat_set.hpp>

using StemId = int32_t;
using TokensSet = boost::container::flat_set<std::string>;
constexpr int32_t MAX_STEMS_NO = 152588;

class StemIdTokensRCUMap
{
public:
    StemIdTokensRCUMap() = default;
    // non copyable
    StemIdTokensRCUMap(const StemIdTokensRCUMap&) = delete;
    StemIdTokensRCUMap& operator=(const StemIdTokensRCUMap&) = delete;

    // thread-safe, non-locking - implements read-copy-update mechanism
    void add_token(StemId id, const std::string& token)
    {
        assert(id < MAX_STEMS_NO);
        bool retry = true;
        while (retry)
        {
            auto tokens_set_oldptr = tokens_rcu_map_[id];
            std::shared_ptr<TokensSet> tokens_set_newptr { nullptr };
            if (tokens_set_oldptr == nullptr)
            {
                tokens_set_newptr = std::make_shared<TokensSet>();
                tokens_set_newptr->insert(token);
            }
            else
            {
                if (tokens_set_oldptr->find(token) != tokens_set_oldptr->end())
                    return; //token is already added
                tokens_set_newptr = std::make_shared<TokensSet>(*tokens_set_oldptr);
                tokens_set_newptr->insert(token);
            }
            //if it wasn't updated swap it with the new one otherwise retry
            retry = !std::atomic_compare_exchange_strong(&(tokens_rcu_map_[id]), &tokens_set_oldptr, tokens_set_newptr);
        }
    }

    std::shared_ptr<TokensSet> get_tokens(StemId id) const
    {
        return tokens_rcu_map_[id];
    }

private:
    std::array<std::shared_ptr<TokensSet>, MAX_STEMS_NO> tokens_rcu_map_;
};

#endif
