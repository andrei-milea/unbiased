#ifndef _ENTRY_PROCESSOR_H
#define _ENTRY_PROCESSOR_H

#include "mongodb.h"
#include "vocabulary.h"
#include "article.h"
#include "article_builder.h"
#include <boost/asio.hpp>
#include <boost/process.hpp>
#include <map>
#include <thread>

namespace bp = boost::process;

class EntryProcessor
{
public:
	EntryProcessor(size_t buff_max_size)
		:buffer_max_size_(buff_max_size),
		article_builder_(MongoDb::get().load_articles_signatures()),
		processed_articles_(0)
	{
	}

	void run(uint32_t threads_no)
	{
		for(size_t idx = 0; idx < threads_no; idx++)
		{
			std::thread new_thread([&]{asio_service_.run();}  );
			new_thread.detach();
		}
	}

	//enqueues build_article
	void scrap_and_process(const std::string& url)
	{
		std::string buffer_str('\0', buffer_max_size_);
		bp::async_pipe apipe(asio_service_);
		bp::child c(bp::search_path("python"), std::string("scrap.py ") + url, bp::std_out > apipe);

		auto get_res = [this, &buffer_str](const boost::system::error_code &ec, std::size_t size)
					{
						if(!ec)
							build_article(buffer_str);
						else
							std::cout << ec.message() << std::endl;
					};
		boost::asio::async_read(apipe, boost::asio::buffer(&buffer_str[0], buffer_str.size()), get_res);
		if(processed_articles_ > Config::get().update_vocab_freq)
		{
			//wait for all articles to be processed
			processed_articles_ = 0;
			article_builder_.save_vocabulary();
		}
	}

private:
	void build_article(const std::string& entry_str)
	{
		try
		{
			Article article;
			BuilderRes result = article_builder_.from_xml(entry_str, article);
			if(result == BuilderRes::VALID || result == BuilderRes::DUPLICATE)
			{
				MongoDb::get().save_article(article);
				processed_articles_++;
			}
		}
		catch(std::exception& ex)
		{
			std::cout << "error processing article: " << ex.what() << std::endl;
		}
	}

	std::string find_source(const std::set<std::string> &duplicates) 
	{
		assert(!duplicates.empty());
		std::vector<std::string> duplicates_vec(duplicates.begin(), duplicates.end());
		auto articles_dates = MongoDb::get().load_articles_dates(duplicates_vec);
		if((articles_dates.size() == duplicates.size()))
			throw std::logic_error("ArticleBuilder::find_source error: failed to retrieve all articles with given ids");
		size_t min_date_idx = 0;
		for(size_t idx = 1; idx < articles_dates.size(); idx++)
		{
			if(stof(articles_dates[idx].second) < stof(articles_dates[min_date_idx].second))	
				min_date_idx = idx;
		}
		return articles_dates[min_date_idx].first;
	}

private:
	boost::asio::io_service asio_service_;
	size_t buffer_max_size_;
	ArticleBuilder article_builder_;
	mutable std::atomic<size_t> processed_articles_;
};

#endif

