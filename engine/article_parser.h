#ifndef _ARTICLE_PARSER_H
#define _ARTICLE_PARSER_H

#include "article.h"
#include "config.h"
#include "vocabulary.h"
#include <boost/locale.hpp>
#include <functional>
#include <type_traits>

class ArticleParser
{
public:
    ArticleParser(const Vocabulary &vocab);

    void parse_from_xml(const std::string& article_xml, Article& article);

    template <typename T = int32_t>
    auto process_tokens(Article& article) const
    {
        std::unordered_set<StemId> stem_ids;
        T shingles{};
        article.tf.resize(vocabulary_.stems_no(), 0.0);

        for (size_t tidx = 0; tidx < article.tokens.size(); tidx++)
        {
            const auto &token = article.tokens[tidx];
            if (!vocabulary_.is_stop_word(token) && token.size() >= 2)
            {
                std::string lower_token = boost::locale::to_lower(token);
                auto stem = lower_token;
                trim_and_stem(stem);
                StemId stem_id = 0;
                if (vocabulary_.get_stem_id(stem, stem_id))
                {
                    //count how many articles contain the token
                    if (stem_ids.count(stem_id) == 0)
                    {
                        vocabulary_.increase_stem_freq(stem_id);
                        stem_ids.insert(stem_id);
                    }
                    vocabulary_.add_token(stem_id, lower_token);
                    //compute term frequency
                    article.tf[stem_id]++;
                    article.ids_tokens_map[stem_id] = tidx;
                }
                else
                    article.unknown_tokens_no++;
            }
            else
            {
                if constexpr (!std::is_integral<T>::value)
                {
                    //compute shigles
                    if (tidx + 2 < article.tokens.size())
                    {
                        auto shingle = get_shingle(article.tokens[tidx], article.tokens[tidx + 1], article.tokens[tidx + 2]);
                        shingles.insert(shingle);
                    }
                }
            }
        }
        return shingles;
    }

private:
    //TODO fix 32-64bit combine
    int32_t get_shingle(const std::string& w1, const std::string& w2, const std::string& w3) const
    {
        std::hash<std::string> hash_fct {};
        uint64_t hash_val = hash_fct(w1);
        uint32_t hash_val1 = hash_fct(w2);
        uint32_t hash_val2 = hash_fct(w3);
        boost::hash_combine(hash_val, hash_val1);
        boost::hash_combine(hash_val, hash_val2);
        return (int32_t)hash_val;
    }

private:
    const Vocabulary& vocabulary_;
    MinHash min_hash_;
};

#endif
