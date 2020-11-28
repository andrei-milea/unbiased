#include "mongodb_helper.h"
#include "config.h"
#include "log_helper.h"
#include "vocabulary.h"
#include "article_utils.h"
#include "bu_clustering.h"
#include "dedup_rpc_stub.h"

#include <exception>
#include <zmq.hpp>
#include <string>

using namespace std;
using namespace bson_bld;

const string words_filename{"words.dat"};
const string stopwords_filename{"stop_words.dat"};

void process_articles_batch(const string& filename_path, Vocabulary& vocab, MongoDb& mongodb_inst)
{
    MinHash minhash{SIGNATURE_SIZE, MINHASH_RANGE_MAX};
    ArticleParser article_parser { vocab };
    std::unique_ptr<Clustering> clustering_processor{make_unique<BU_Clustering>(vocab)};

    zmq::context_t zmq_ctx{1};
    DedupRPCStub dedup_rpc_stub{zmq_ctx};

    vector<pair<string, Signature>> docs_signatures;
    vector<Article> articles;

    auto articles_xml = load_articles_xml(filename_path);
    for(const auto& article_xml : articles_xml)
    {
        Article new_article;
        article_parser.parse_from_xml(article_xml, new_article);

        set<string> stems;
        bool valid = article_parser.tokenize_validate(new_article, stems);
        if (valid)
        {
            vocab.add_stems(std::move(stems));
            articles.push_back(std::move(new_article));
        }
        else
        {
            save_invalid_url(mongodb_inst, new_article.meta_data.url);
            spdlog::info("invalid article url - {}, title - {}", new_article.meta_data.url, new_article.meta_data.title);
            continue;
        }
    }

    spdlog::info("articles: {} valid articles: {}", articles_xml.size(), articles.size());
    std::vector<ProcessedArticle> processed_articles;
    for (size_t idx = 0; idx < articles.size(); idx++)
    {
        ProcessedArticle proc_article;
        auto shingles = article_parser.process_tokens<set<int32_t>>(articles[idx], proc_article);

        string id = save_article(mongodb_inst, proc_article);
        proc_article.signature = minhash.compute_signature(shingles);
        if (!dedup_rpc_stub.is_duplicate(make_pair(id, proc_article.signature)))
            processed_articles.push_back(std::move(proc_article));
    }

    clustering_processor->create_clusters(processed_articles);
}

int main()
{
    try
    {
        spdlog::set_default_logger(std::make_shared<spdlog::logger>("runtime_logger", std::make_shared<spdlog::sinks::daily_file_sink_mt>("clustering_startup.log", 23, 59)));

        MongoDb mongodb_inst{Config::get().mongo_credentials.dbname};
        mongodb_inst.drop_collection("articles");
        Vocabulary vocab{ words_filename, stopwords_filename };
        save_vocabulary(mongodb_inst, vocab);
        process_articles_batch("articles.xml", vocab, mongodb_inst);
    }
    catch (const std::exception& ex)
    {
        spdlog::error("quiting, clustering exception: {}", ex.what());
    }
    return 0;
}

