#ifndef _LSA_H
#define _LSA_H

//use lapack library
#ifndef DLIB_USE_LAPACK
#define DLIB_USE_LAPACK
#endif

#include "log_helper.h"
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

    void run_svd(const std::vector<ProcessedArticle>& articles);

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

    dlib::matrix<double> foldin_doc(const ProcessedArticle& art) const;

    std::vector<std::set<std::string>> get_top_concepts(const std::vector<ProcessedArticle>& articles, int64_t concepts, int64_t terms) const;

    void build_term_doc_matrix(const std::vector<ProcessedArticle>& articles, int32_t tf_thresold = 3);

    //////debug api
    //void print_top_concepts(const std::vector<ProcessedArticle>& articles, double threshold = 0.03) const;

    void print_term_doc_matrix() const;

    void print_sigma() const;

private:

    void init_term_doc_matrix(size_t rows, size_t cols)
    {
        term_doc_mat_.set_size(rows, cols);
        for (long row = 0; row < long(rows); row++)
            for (long col = 0; col < long(cols); col++)
                term_doc_mat_(row, col) = 0;
    }

private:
    const Vocabulary& vocabulary_;

    //index of an element in this vector corresponds to a stem id in the vocabulary
    std::vector<size_t> terms_mat_to_vocab_;

    dlib::matrix<double> term_doc_mat_;
    
    //the following matrices contain the SVD of the terms x docs matrix
    dlib::matrix<double> U_;
    dlib::matrix<double> sigma_;
    dlib::matrix<double> V_;
    double articles_no_;
};

#endif
