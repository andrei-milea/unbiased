#ifndef _STEMID_TOKENS_RCUMAP_H
#define _STEMID_TOKENS_RCUMAP_H

#include <atomic>
#include <array>
#include <boost/container/flat_set.hpp>

using WordId = int32_t;
using TokensSet = boost::container::flat_set<std::string>;
constexpr int32_t MAX_WORDS_NO = 152588;

class StemIdTokensRCUMap
{
public:
    StemIdTokensRCUMap() = default;
    // non copyable
    StemIdTokensRCUMap(const StemIdTokensRCUMap &) = delete;
    StemIdTokensRCUMap& operator=(const StemIdTokensRCUMap &) = delete;

    // thread-safe, non-locking - implements read-copy-update mechanism
    void add_token(WordId id, const std::string & token)
    {
        assert(id < MAX_WORDS_NO);
        bool retry = true;
        while(retry)
        {
            auto tokens_set_oldptr = tokens_rcu_map_[id];
            std::shared_ptr<TokensSet> tokens_set_newptr{nullptr};
            if (tokens_set_oldptr == nullptr)
            {
                tokens_set_newptr = std::make_shared<TokensSet>();
                tokens_set_newptr->insert(token);
            }
            else
            {
                if(tokens_set_oldptr->find(token) != tokens_set_oldptr->end())
                    return; //token is already added
                tokens_set_newptr = std::make_shared<TokensSet>(*tokens_set_oldptr);
                tokens_set_newptr->insert(token);
            }
            //if it wasn't updated swap it with the new one otherwise retry
            retry = !std::atomic_compare_exchange_strong(&(tokens_rcu_map_[id]), &tokens_set_oldptr, tokens_set_newptr);
        }
    }

    std::shared_ptr<TokensSet> get_tokens(WordId id) const
    {
        return tokens_rcu_map_[id];
    }
    
private:
    std::array<std::shared_ptr<TokensSet>, MAX_WORDS_NO> tokens_rcu_map_;
};

#endif
