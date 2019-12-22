#ifndef _LSA_H
#define _LSA_H

#define DLIB_USE_LAPACK	//use LAPACK library

#include "vocabulary.h"
#include "article.h"
#include <vector>
#include <dlib/matrix.h>
#include <unordered_map>

class LSA
{
public:
	LSA(const Vocabulary& vocabulary)
	:vocabulary_(vocabulary)
	{}

	void print_term_doc_matrix()const;

	void print_sigma()const;

	void run_svd(const std::vector<Article> &articles, size_t all_articles_no);

	const dlib::matrix<double>& get_terms_concepts_mat()const
	{
		return U_;
	}

	const dlib::matrix<double>& get_docs_concepts_mat()const
	{
		return V_;
	}

	//threshold is 2 standard deviations from the mean
	double get_terms_threshold(size_t concepts_no)const
	{
		assert(concepts_no <= U_.nc());
		const dlib::matrix<double> mat = dlib::subm(U_, 0, U_.nr(), 0, concepts_no);
		double m = dlib::mean(mat);
		double sd = dlib::stddev(mat);
		std::cout << "mean: " << m << "\n";
		std::cout << "stddev: " << sd << "\n";
		return  m + 2 * sd;
	}

	std::string get_word(size_t word_id)const
	{
		std::string keyword;
		bool res = vocabulary_.get_word(word_id, keyword);
		assert(res);
		return keyword;
	}

	std::vector<std::set<size_t>> get_docs_keywords(float doc_threshold, float term_threshold, size_t concepts_no)const;

	dlib::matrix<double> foldin_doc(const Article& art, size_t concepts_no)const;

	void print_top_concepts(const std::vector<Article> &articles, double threshold = 0.03)const;

private:
	void build_term_doc_matrix(const std::vector<Article> &articles, size_t all_articles_no);

	void build_vocab_to_mat_idx(const std::vector<Article> &articles)	
	{
		assert(articles.size() > 0);
		size_t terms_no = articles[0].tf.size();
		assert(vocabulary_.words_no() == terms_no);
		for(size_t ridx = 0; ridx < terms_no; ridx++)
		{
			for(size_t cidx = 0; cidx < articles.size(); cidx++)
			{
				if(articles[cidx].tf[ridx] != 0.0)
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
		for(long row = 0; row < long(rows); row++)
			for(long col = 0; col < long(cols); col++)
				term_doc_mat_(row, col) = 0;
	}

private:
	const Vocabulary& 		vocabulary_;
	std::vector<size_t>		terms_mat_to_vocab_;
	dlib::matrix<double> 	term_doc_mat_;
	dlib::matrix<double> 	U_;
	dlib::matrix<double> 	sigma_;
	dlib::matrix<double> 	V_;
	double 					articles_no_;
};


#endif

