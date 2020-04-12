#ifndef _LSA_H
#define _LSA_H

#define DLIB_USE_LAPACK //use LAPACK library

#include "utils/log_helper.h"
#include "article.h"
#include "vocabulary.h"
#include <dlib/matrix.h>
#include <string>
#include <vector>

class LSA
{
public:
    LSA(const Vocabulary& vocabulary)
        : vocabulary_(vocabulary)
    {
    }

    void run_svd(const std::vector<Article>& articles, float tf_threshold = 1.0);

    const dlib::matrix<double>& get_terms_concepts_mat() const
    {
        return U_;
    }

    const dlib::matrix<double>& get_docs_concepts_mat() const
    {
        return V_;
    }

    const dlib::matrix<double>& get_term_doc_matrix() const
    {
        return term_doc_mat_;
    }

    //threshold is 2 standard deviations from the mean
    double get_terms_threshold(int64_t concepts_no) const
    {
        assert(concepts_no <= U_.nc());
        const dlib::matrix<double> mat = dlib::subm(U_, 0, U_.nr(), 0, concepts_no);
        double m = dlib::mean(mat);
        double sd = dlib::stddev(mat);
        std::cout << "mean: " << m << "\n";
        std::cout << "stddev: " << sd << "\n";
        return m + 2 * sd;
    }

    std::string get_word(size_t word_id) const
    {
        std::string keyword;
        bool res = vocabulary_.get_stem(word_id, keyword);
        assert(res);
        return keyword;
    }

    std::vector<std::set<size_t>> get_docs_keywords(float doc_threshold, float term_threshold, int64_t concepts_no) const;

    dlib::matrix<double> foldin_doc(const Article& art) const;

    std::vector<std::set<std::string>> get_top_concepts(const std::vector<Article>& articles, int64_t concepts, int64_t terms) const;

    void build_term_doc_matrix(const std::vector<Article>& articles, float tf_threshold = 1.0);

    //////debug api
    void print_top_concepts(const std::vector<Article>& articles, double threshold = 0.03) const;

    void print_term_doc_matrix() const;

    void print_sigma() const;

private:
    //filter out terms that are not in any article
    //this index contains all term ids currently in use
    void build_vocab_to_mat_idx(const std::vector<Article>& articles, float tf_threshold = 3.0)
    {
        assert(articles.size() > 0);
        size_t terms_no = articles[0].tf.size();
        assert(vocabulary_.stems_no() == terms_no);
        spdlog::info("building terms matrix to vocab index terms {}, articles {}", terms_no, articles.size());
        for (size_t ridx = 0; ridx < terms_no; ridx++)
        {
            for (size_t cidx = 0; cidx < articles.size(); cidx++)
            {
                if (articles[cidx].tf[ridx] >= tf_threshold && articles[cidx].tf[ridx] < articles.size()/2)
                {
                    terms_mat_to_vocab_.push_back(ridx);
                    break;
                }
            }
        }
    }

    void init_term_doc_matrix(size_t rows, size_t cols)
    {
        term_doc_mat_.set_size(rows, cols);
        for (long row = 0; row < long(rows); row++)
            for (long col = 0; col < long(cols); col++)
                term_doc_mat_(row, col) = 0;
    }

private:
    const Vocabulary& vocabulary_;
    std::vector<size_t> terms_mat_to_vocab_;
    dlib::matrix<double> term_doc_mat_;
    dlib::matrix<double> U_;
    dlib::matrix<double> sigma_;
    dlib::matrix<double> V_;
    double articles_no_;
};

#endif
