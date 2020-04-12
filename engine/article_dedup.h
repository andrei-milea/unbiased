#ifndef _ARTICLE_DEDUP_H
#define _ARTICLE_DEDUP_H

#include "mongodb_helper.h"
#include "config.h"
#include "minhash.h"
#include "lsh_deduplication.h"
#include "utils/log_helper.h"

#include<string>
#include<utility>

class ArticleDedup
{
public:
    ArticleDedup()
        : mongodb_inst_(Config::get().mongo_credentials.dbname)
        , lsh_deduplication_(SIGNATURE_SIZE, load_articles_signatures(mongodb_inst_))
    {}

    bool is_duplicate(const std::pair<std::string, Signature> &id_sign)
    {
        auto duplicates = lsh_deduplication_.process_signature(id_sign);
        if (!duplicates.empty())
        {
            std::string src_id = find_source(duplicates);
            spdlog::info("found article duplicate for {}: {}", id_sign.first, src_id);
            //TODO store this info in db
            return true;
        }
        return false;
    }

private:
    //retrieves id of the first article duplicate from mongodb based on date
    std::string find_source(const std::set<std::string> &duplicates)
    {
        assert(!duplicates.empty());
        std::vector<std::string> duplicates_vec(duplicates.begin(), duplicates.end());
        auto articles_dates = load_articles_dates(mongodb_inst_, duplicates_vec);
        size_t min_date_idx = 0;
        for(size_t idx = 1; idx < articles_dates.size(); idx++)
        {
            if(std::stof(articles_dates[idx].second) < std::stof(articles_dates[min_date_idx].second))	
                min_date_idx = idx;
        }
        return articles_dates[min_date_idx].first;
    }

private:
    MongoDb mongodb_inst_;
    LSHDeduplication lsh_deduplication_;
};



#endif
