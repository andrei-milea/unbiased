#ifndef _BU_CLUSTERING
#define _BU_CLUSTERING

#include "clustering.h"
#include "lsa.h"

//bottom up aglomerative clustering
class BU_Clustering : public Clustering
{
public:
	explicit BU_Clustering(const Vocabulary& vocab)
	:lsa_proc_{vocab}
	{}

	void create_clusters(const std::vector<Article> &articles) override;

	size_t cluster(const Article &article) override;

protected:
	template<class FUNC>
	void build_dist_mat(const std::vector<Article> &articles, FUNC dist_fnc)
	{
		lsa_proc_.run_svd(articles, articles.size());
		const auto& docs_concepts_mat = lsa_proc_.get_docs_concepts_mat();
		dist_mat_.set_size(docs_concepts_mat.nr(), docs_concepts_mat.nr());
		for(long row = 0; row < dist_mat_.nr(); row++)
			for(long col = 0; col < dist_mat_.nc(); col++)
				dist_mat_(row, col) = dist_fnc(dlib::rowm(docs_concepts_mat, row), dlib::rowm(docs_concepts_mat, col));
	}

protected:
	LSA lsa_proc_;
	dlib::matrix<double> dist_mat_;
};

#endif
