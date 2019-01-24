#ifndef _ENTRY_PROCESSOR_H
#define _ENTRY_PROCESSOR_H

#include "vocabulary.h"
#include "article.h"
#include "article_builder.h"
#include <boost/asio.hpp>
#include <boost/process.hpp>
#include <map>
#include <thread>

class Clustering;

class EntryProcessor
{
public:
	explicit EntryProcessor(size_t buff_max_size);

	void init();

	//start threads
	void run(uint32_t threads_no);

	//enqueues process_article
	void scrap_and_process(const std::string& url);

private:
	void process_article(const std::string& entry_str) noexcept;

	/*std::string find_source(const std::set<std::string> &duplicates) 
	{
		assert(!duplicates.empty());
		std::vector<std::string> duplicates_vec(duplicates.begin(), duplicates.end());
		auto articles_dates = article_builder_.load_articles_dates(duplicates_vec);
		if((articles_dates.size() == duplicates.size()))
			throw std::logic_error("ArticleBuilder::find_source error: failed to retrieve all articles with given ids");
		size_t min_date_idx = 0;
		for(size_t idx = 1; idx < articles_dates.size(); idx++)
		{
			if(stof(articles_dates[idx].second) < stof(articles_dates[min_date_idx].second))	
				min_date_idx = idx;
		}
		return articles_dates[min_date_idx].first;
	}*/

private:
	boost::asio::io_service asio_service_;
	size_t buffer_max_size_;
	ArticleBuilder article_builder_;
	mutable std::atomic<size_t> processed_articles_;
	std::unique_ptr<Clustering> cluster_proc_;
};

#endif

