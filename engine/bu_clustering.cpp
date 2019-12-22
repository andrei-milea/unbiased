#include "bu_clustering.h"
#include <dlib/graph_utils.h>
#include <dlib/clustering.h>
#include <vector>
#include <unordered_map>

using namespace std;

void BU_Clustering::create_clusters(const std::vector<Article> &articles)
{
	unordered_map<size_t, vector<size_t>> topics_arts_map;
	build_dist_mat(articles, dlib::cosine_distance{});
	vector<unsigned long> labels(dist_mat_.nr());
	size_t clusters_no = dlib::bottom_up_cluster(dist_mat_, labels, 50, 0.5);
	for(size_t idx = 0; idx < articles.size(); idx++)
		topics_arts_map[labels[idx]].push_back(idx);


	//const auto& docs_concepts_mat = lsa_proc_.get_docs_concepts_mat();
	double terms_th = lsa_proc_.get_terms_threshold(50);
	cout << "topics: " << clusters_no << " " << topics_arts_map.size() << endl;
	unordered_map<size_t, vector<string>> topics_terms_map;
	vector<set<size_t>> docs_keywords = lsa_proc_.get_docs_keywords(terms_th, 0.11, 50);
	for(const auto& topic_art : topics_arts_map)
	{
		cout << "topic\n";
		for(size_t art_idx : topic_art.second)
		{
			cout << "title: " << articles[art_idx].title << endl;
			for(size_t kidx : docs_keywords[art_idx])
			{
				string keyword = lsa_proc_.get_word(kidx);
				cout << "key: " << keyword << endl;
				topics_terms_map[topic_art.first].push_back(keyword);
			}
		}
		cout << "\n\n";
	}
	cout << "-----------------------\n";
	for(const auto& topic_terms : topics_terms_map)
	{
		cout << "topic\n";
		for(const string& k : topic_terms.second)
			cout << "key: " << k << endl;
	}
}

size_t BU_Clustering::cluster(const Article &article)
{
	//TODO
}

