#include "lsa.h"
#include "utils/perf_clock.h"
#include <algorithm>
#include <math.h>
#include <unordered_map>

using namespace dlib;
using namespace std;

void LSA::build_term_doc_matrix(const std::vector<ProcessedArticle>& articles, int32_t tf_thresold)
{
    assert(articles.size() > 0);
    articles_no_ = double(articles.size());
    init_term_doc_matrix(vocabulary_.stems_no(), articles.size());

    spdlog::info("building term_doc_matrix {} x {}", vocabulary_.stems_no(), articles.size());
    for (size_t cidx = 0; cidx < articles.size(); cidx++)
    {
        // compute term frequency
        std::vector<int32_t> tf(vocabulary_.stems_no(), 0);
        for (StemId stem : articles[cidx].stem_ids)
        {
            assert(stem <= vocabulary_.stems_no());
            tf[stem]++;
        }

        for (size_t ridx = 0; ridx < vocabulary_.stems_no(); ridx++)
        {
            assert(articles[cidx].tokens_no != 0);
            if (tf[ridx] > tf_thresold)
                term_doc_mat_(ridx, cidx) = tf[ridx] / articles[cidx].tokens_no;
            //* log(articles_no_ / vocabulary_.get_stem_freq(ridx));
            assert(!isnan(term_doc_mat_(ridx, cidx)));
        }
    }
}

void LSA::print_term_doc_matrix() const
{
    cout << "term_doc_mat size: " << term_doc_mat_.nr() << " " << term_doc_mat_.nc() << "\n";
    for (auto row = 0; row < term_doc_mat_.nr(); row++)
    {
        for (auto col = 0; col < term_doc_mat_.nc(); col++)
            cout << term_doc_mat_(row, col) << " ";
        cout << "\n";
    }
}

void LSA::print_sigma() const
{
    cout << "sigma size: " << sigma_.nr() << " " << sigma_.nc() << "\n";
    spdlog::info("LSA::print_sigma {} x {}", sigma_.nr(), sigma_.nc());
    for (auto row = 0; row < sigma_.nr(); row++)
    {
        for (auto col = 0; col < sigma_.nc(); col++)
            if(row == col)
                spdlog::info("{} ", sigma_(row, col));
            else
                assert(sigma_(row, col) == 0.0);
    }
}

void LSA::run_svd(const std::vector<ProcessedArticle>& articles)
{
    LogRunTimeRAII log_runtime("LSA::run_svd");
    build_term_doc_matrix(articles, 3);
    svd(term_doc_mat_, U_, sigma_, V_);
}

std::vector<std::set<size_t>> LSA::get_docs_keywords(float doc_threshold, float term_threshold, int64_t concepts_no) const
{
    std::vector<std::set<size_t>> docs_keywords(V_.nr());
    for (auto doc_idx = 0; doc_idx < V_.nr(); doc_idx++)
    {
        for (int64_t concept_idx = 0; concept_idx < concepts_no; concept_idx++)
        {
            if (V_(doc_idx, concept_idx) > doc_threshold)
            {
                for (auto term_idx = 0; term_idx < U_.nr(); term_idx++)
                    if (U_(term_idx, concept_idx) > term_threshold)
                        docs_keywords[doc_idx].insert(term_idx);
            }
        }
    }
    return docs_keywords;
}

dlib::matrix<double> LSA::foldin_doc(const ProcessedArticle& art) const
{
    //TODO
    dlib::matrix<double> doc_vec;
    /*
    for (size_t ridx = 0; ridx < vocabulary_.stems_no(); ridx++)
    {
        doc_vec(ridx) = art.tf[vocab_idx] / art.tokens_no
            * log(articles_no_ / vocabulary_.get_stem_freq(vocab_idx));
    }*/
    return doc_vec;
}

/*void LSA::print_top_concepts(const std::vector<ProcessedArticle>& articles, double threshold) const
{
    unordered_map<size_t, std::vector<size_t>> concept_doc_map;
    unordered_map<size_t, std::vector<size_t>> concept_term_map;
    cout << "U_ size: " << U_.nr() << " " << U_.nc() << "\n";

    //print concepts - terms
    //rows are terms, cols are concepts
    for (long col = 0; col < U_.nc(); col++)
    {
        cout << "CONCEPT START\n";
        for (long row = 0; row < U_.nr(); row++)
        {
            if (U_(row, col) > threshold)
            {
                string stem;
                bool res = vocabulary_.get_stem(terms_mat_to_vocab_[row], stem);
                concept_term_map[col].push_back(terms_mat_to_vocab_[row]);
                assert(res);
                cout << stem << " ";
            }
            if (row < V_.nr() && V_(row, col) > threshold)
                concept_doc_map[col].push_back(row);
        }
        cout << "CONCEPT END\n";
    }

    //print concepts - articles
    for (const auto& concept_doc : concept_doc_map)
    {
        cout << "CONCEPT--" << endl;
        for (const auto art_idx : concept_doc.second)
        {
            cout << "article: " << articles[art_idx].title << "\n";
            for (const auto& word_idx : concept_term_map[concept_doc.first])
            {
                auto it = articles[art_idx].ids_tokens_map.find(word_idx);
                if (it != articles[art_idx].ids_tokens_map.end())
                {
                    cout << articles[art_idx].tokens[it->second] << endl;
                }
            }
        }
        cout << endl;
    }
}*/

std::vector<std::set<string>> LSA::get_top_concepts(const std::vector<ProcessedArticle>& articles, int64_t concepts, int64_t terms) const
{
    spdlog::info("LSA::get_top_concepts U {} x {}, concepts_no {}, terms no {}", U_.nr(), U_.nc(), concepts, terms);
    LogRunTimeRAII log_runtime("LSA::get_top_concepts");
    //rows are terms, cols are concepts
    concepts = min(concepts, U_.nc());
    std::vector<std::set<string>> concepts_vec;
    for (long col = 0; col < concepts; col++)
    {
        std::vector<pair<string, double>> concept_terms_vals;
        std::set<string> concept_terms;

        for (long row = 0; row < U_.nr(); row++)
        {
            auto tokens_set = vocabulary_.get_tokens(row);
            assert(tokens_set && !tokens_set->empty());
            string term = *(tokens_set->begin());
            concept_terms_vals.push_back(make_pair(term, U_(row, col)));
        }

        //get most important terms for each topic
        sort(concept_terms_vals.begin(), concept_terms_vals.end(),
            [](const pair<string, double>& elem1, const pair<string, double>& elem2) { return elem1.second > elem2.second; });
        size_t min_sz = min(concept_terms_vals.size(), size_t(terms));
        transform(concept_terms_vals.begin(), concept_terms_vals.begin() + min_sz, inserter(concept_terms, concept_terms.end()),
            [](const auto& pair) { return pair.first; });

        concepts_vec.push_back(concept_terms);
    }
    return concepts_vec;
}
