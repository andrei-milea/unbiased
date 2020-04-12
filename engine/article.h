#ifndef _ARTICLE_H
#define _ARTICLE_H

#include "minhash.h"
#include <iostream>
#include <set>
#include <string>
#include <unordered_map>
#include <vector>

class Article
{
public:

    bool operator==(const Article& article) const noexcept
    {
        return /*id == article.id &&*/ url == article.url && title == article.title && source == article.source
            && authors == article.authors && length == article.length && signature == article.signature
            && tf == article.tf && keywords == article.keywords && entities == article.entities;
    }

    bool is_valid(int32_t min_tokens = 100, float invalid_tokens_threshold = 0.3) const
    {
        return tokens.size() >= min_tokens && float(unknown_tokens_no + 1.0) / tokens.size() <= invalid_tokens_threshold;
    }

    std::string id;
    std::string url;
    std::string title;
    std::string date;
    std::string source;
    std::vector<std::string> authors;
    std::vector<std::string> tokens;
    size_t length = 0;
    int32_t unknown_tokens_no = 0;
    Signature signature;
    std::unordered_map<size_t, size_t> ids_tokens_map;
    std::vector<double> tf;
    std::vector<std::string> keywords;
    std::vector<std::string> entities;
};


inline std::ostream& operator <<(std::ostream &out, const Article& art)
{
    out << art.id << art.title << "\n";
    return out;
}

#endif
