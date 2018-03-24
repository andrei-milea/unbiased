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
	LSA(const std::vector<Article> &articles, size_t articles_no, const Vocabulary& vocabulary)
	:vocabulary_(vocabulary)
	{
		assert(articles.size() > 0);
		double d_articles_no = double(articles_no);
		size_t tf_size = articles[0].tf.size();
		term_doc_mat_.set_size(tf_size, articles.size());
		for(size_t cidx = 0; cidx < articles.size(); cidx++)
		{
			for(size_t ridx = 0; ridx < tf_size; ridx++)
			{
				if(articles[cidx].tf[ridx] != 0.0)
					term_doc_mat_(ridx, cidx) = articles[cidx].tf[ridx] * std::log(d_articles_no / (vocabulary_.get_word_freq(ridx) + 1.0));
			}
		}
	}

	void run_svd();


private:
	const Vocabulary& 		vocabulary_;
	dlib::matrix<double> 	term_doc_mat_;
	dlib::matrix<double> 	U_;
	dlib::matrix<double> 	sigma_;
	dlib::matrix<double> 	V_ ;
};


#endif

