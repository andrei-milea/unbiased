#ifndef _CLUSTERING_H
#define _CLUSTERING_H

#include "article.h"
#include "utils/tokenize.h"
#include "article_parser.h"
#include <string>
#include <vector>

struct Cluster
{
    std::vector<std::string> articles_ids;
    std::vector<std::string> topics;
    std::vector<std::string> entities;
};

//base class for clustering
class Clustering
{
public:
    virtual ~Clustering() = default;

    virtual void create_clusters(const std::vector<ProcessedArticle>& articles) = 0;

    virtual size_t add_to_clusters(const ProcessedArticle& article) = 0;

    const std::vector<Cluster>& get_clusters() const noexcept
    {
        return clusters_;
    }

    //and add token to stemid->tokens map
    //TODO - improve this
    /*for (const auto& idx_token : article.ids_tokens_map)
    {
        vocabulary_.increase_word_freq(idx_token.first);
        vocabulary_.add_token(idx_token.first, boost::locale::to_lower(article.tokens[idx_token.second]));
    }*/

private:
    std::vector<Cluster> clusters_;
};

#endif
