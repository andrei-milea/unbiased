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
		init_term_doc_matrix(terms_mat_to_vocab_.size(), articles.size());
		for(size_t cidx = 0; cidx < articles.size(); cidx++)
		{
			for(size_t ridx = 0; ridx < terms_mat_to_vocab_.size(); ridx++)
			{
				size_t vocab_idx = terms_mat_to_vocab_.at(ridx);
				if(articles[cidx].tf[vocab_idx] != 0.0)
					term_doc_mat_(ridx, cidx) = articles[cidx].tf[vocab_idx] * std::log(d_articles_no / vocabulary_.get_word_freq(vocab_idx));
			}
		} 
	}

	void print_term_doc_matrix()const
	{
		std::cout << "term_doc_mat size: " << term_doc_mat_.nr() << " " << term_doc_mat_.nc() << "\n";
		for(long row = 0; row < term_doc_mat_.nr(); row++)
		{
			for(long col = 0; col < term_doc_mat_.nc(); col++)
				std::cout << term_doc_mat_(row, col) << " ";
			std::cout << "\n";
		}
	}

	void print_sigma()const
	{
		std::cout << "sigma size: " << sigma_.nr() << " " << sigma_.nc() << "\n";
		for(long row = 0; row < sigma_.nr(); row++)
		{
			for(long col = 0; col < sigma_.nc(); col++)
				std::cout << sigma_(row, col) << " ";
			std::cout << "\n";
		}
	}

	void print_top_concepts(double threshold = 0.03)
	{
		std::cout << "U_ size: " << U_.nr() << " " << U_.nc() << "\n";
		//rows are terms, cols are concepts
		for(long col = 0; col < U_.nc(); col++)
		{
			std::cout << "CONCEPT START\n";
			for(long row = 0; row < U_.nr(); row++)
			{
				if(U_(row, col) > threshold)
				{
					std::string word;
					bool res = vocabulary_.get_word(terms_mat_to_vocab_[row], word);
					assert(res);
					std::cout << word << " ";
				}
			}
			std::cout << "CONCEPT END\n";
		}
	}

	void run_svd();

private:
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

