#ifndef _LSA_H
#define _LSA_H

#define DLIB_USE_LAPACK	//use LAPACK library

#include "vocabulary.h"
#include "article.h"
#include <vector>
#include <dlib/matrix.h>


class LSA
{
public:
	LSA(const Vocabulary& vocabulary)
	:vocabulary_(vocabulary)
	{
	}

	void build_term_doc_matrix(const std::vector<Article> &articles, size_t all_articles_no)
	{
		assert(articles.size() > 0 && all_articles_no > 0);
		double d_articles_no = double(all_articles_no);
		build_vocab_to_mat_idx(articles);
		term_doc_mat_.set_size(terms_mat_to_vocab_.size(), articles.size());
		for(size_t cidx = 0; cidx < articles.size(); cidx++)
		{
			for(size_t ridx = 0; ridx < terms_mat_to_vocab_.size(); ridx++)
			{
				size_t vocab_idx = terms_mat_to_vocab_.at(ridx);
				if(articles[cidx].tf[vocab_idx] != 0.0)
					term_doc_mat_(ridx, cidx) = articles[cidx].tf[vocab_idx] * std::log(d_articles_no / (vocabulary_.get_word_freq(vocab_idx) + 1.0));
			}
		} 
	}

	void run_svd();

private:
	void build_vocab_to_mat_idx(const std::vector<Article> &articles)	
	{
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

private:
	const Vocabulary& 		vocabulary_;
	std::vector<size_t>		terms_mat_to_vocab_;
	dlib::matrix<double> 	term_doc_mat_;
	dlib::matrix<double> 	U_;
	dlib::matrix<double> 	sigma_;
	dlib::matrix<double> 	V_;
};


#endif

