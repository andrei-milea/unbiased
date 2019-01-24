#ifndef _CLUSTERING_H
#define _CLUSTERING_H

#include "article.h"
#include <vector>
#include <string>

struct Cluster
{
	std::vector<std::string> articles_ids;
	std::vector<std::string> topics;
	std::vector<std::string> entities;
};

//base class for clustering
class Clustering
{
public:
	virtual ~Clustering()=default;

	virtual void create_clusters(const std::vector<Article> &articles)=0;

	virtual size_t cluster(const Article &article)=0;

	const std::vector<Cluster>& get_clusters()const noexcept
	{
		return clusters_;
	}

private:
	std::vector<Cluster> clusters_;
};

#endif

