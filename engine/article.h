#ifndef _ARTICLE_H
#define _ARTICLE_H

#include "minhash.h"
#include "vocabulary.h"
#include <iostream>
#include <set>
#include <string>
#include <unordered_map>
#include <vector>

struct ArticleMetaData
{
    std::string id;
    std::string url;
    std::string title;
    std::string date;
    std::string source;
    std::vector<std::string> authors;

    bool operator==(const ArticleMetaData& article_meta) const noexcept
    {
        return id == article_meta.id && url == article_meta.url && title == article_meta.title
            && source == article_meta.source && authors == article_meta.authors;
    }
};

struct Article
{
    std::string text;
    std::vector<std::string> tokens; //TODO should be string_view
    ArticleMetaData meta_data;
};

struct ProcessedArticle
{
    bool operator==(const ProcessedArticle& article) const noexcept
    {
        return meta_data == article.meta_data && signature == article.signature
            && stem_ids == article.stem_ids && entities == article.entities;
    }

    Signature signature;
    std::vector<StemId> stem_ids;
    std::unordered_map<size_t, size_t> ids_tokens_map;
    std::vector<std::string> entities;
    size_t tokens_no = 0;
    size_t length = 0;
    int32_t unknown_tokens_no = 0;
    ArticleMetaData meta_data;
};

inline std::ostream& operator <<(std::ostream &out, const Article& art)
{
    out << art.meta_data.id << art.meta_data.title << "\n";
    return out;
}

inline std::ostream& operator<<(std::ostream& out, const ProcessedArticle& art)
{
    out << art.meta_data.id << art.meta_data.title << "\n";
    return out;
}

#endif
