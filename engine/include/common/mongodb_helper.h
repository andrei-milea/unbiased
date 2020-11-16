#ifndef _MONGODB_HELPER_H
#define _MONGODB_HELPER_H

#include "mongodb.h"
#include "bson_builder.h"

inline void save_vocabulary(MongoDb& mongodb_inst, const Vocabulary& vocab)
{
    auto doc = bson_bld::to_bson(vocab);
    mongodb_inst.drop_collection("vocabulary");
    mongodb_inst.save_doc("vocabulary", doc);
}

inline void load_vocabulary(MongoDb& mongodb_inst, Vocabulary& vocab)
{
    auto result = mongodb_inst.get_docs("vocabulary");
    bson_bld::from_bson(*(result.begin()), vocab);
}

inline void update_vocab_freq(MongoDb& mongodb_inst, Vocabulary& vocab)
{
    auto doc = bson_bld::to_bson(vocab, true);
    mongodb_inst.update_doc("vocabulary", doc);
}

inline std::string save_article(MongoDb& mongodb_inst, const ProcessedArticle& art)
{
    auto doc = bson_bld::to_bson(art);
    return mongodb_inst.save_doc("articles", doc);
}

inline void save_invalid_url(MongoDb& mongodb_inst, const std::string& url)
{
    bson_doc_bld bson_doc_builder;
    auto doc = bson_doc_builder << "url" << url << bsoncxx::builder::stream::finalize;
    mongodb_inst.save_doc("invalid_urls", doc);
}

std::vector<std::pair<std::string, std::string>> load_articles_dates(MongoDb& mongodb_inst, const std::vector<std::string>& articles_ids);

std::vector<std::pair<std::string, Signature>> load_articles_signatures(MongoDb& mongodb_inst);

#endif
