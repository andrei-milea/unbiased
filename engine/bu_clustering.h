#ifndef _BU_CLUSTERING
#define _BU_CLUSTERING

#include "clustering.h"
#include "lsa.h"

//bottom up aglomerative clustering
class BU_Clustering : public Clustering
{
public:
    explicit BU_Clustering(const Vocabulary& vocab)
        : lsa_proc_ { vocab }
    {
    }

    void create_clusters(const std::vector<Article>& articles) override;

    size_t cluster(const Article& article) override;

protected:
    template <class FUNC>
    void build_dist_mat(const std::vector<Article>& articles, FUNC&& dist_fnc)
    {
        lsa_proc_.run_svd(articles, articles.size());
        const auto& docs_concepts_mat = lsa_proc_.get_docs_concepts_mat();
        dist_mat_.set_size(docs_concepts_mat.nr(), docs_concepts_mat.nr());
        for (long di = 0; di < dist_mat_.nr(); di++)
        {
            for (long dj = di + 1; dj < dist_mat_.nr(); dj++)
            {
                const dlib::matrix<double> row_i = dlib::subm(docs_concepts_mat, di, 0, 1, 50);
                //std::cout << "doc-concepts: " << row_i << std::endl;
                const dlib::matrix<double> row_j = dlib::subm(docs_concepts_mat, dj, 0, 1, 50);
                dist_mat_(di, dj) = dist_fnc(row_i, row_j);
                dist_mat_(dj, di) = dist_mat_(di, dj);
                //dist_mat_(di, di) = dist_mat_(di, dj);
            }
        }
    }

protected:
    LSA lsa_proc_;
    dlib::matrix<double> dist_mat_;
};

#endif
