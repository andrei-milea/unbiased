#include "bu_clustering.h"
#include <dlib/graph_utils.h>
#include <dlib/clustering.h>
#include <vector>

using namespace std;

void BU_Clustering::create_clusters(const std::vector<Article> &articles)
{
	build_dist_mat(articles, dlib::cosine_distance{});
	vector<unsigned long> labels(dist_mat_.nr());
	dlib::bottom_up_cluster(dist_mat_, labels, 20);
	//TODO
}

size_t BU_Clustering::cluster(const Article &article)
{
	//TODO
}

