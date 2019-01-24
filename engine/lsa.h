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

	void print_top_concepts(const std::vector<Article> &articles, double threshold = 0.03);

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
};


#endif

