#ifndef _ARTICLE_PROCESSOR_H
#define _ARTICLE_PROCESSOR_H

#include "dictionary.h"
#include "similarity.h"
#include "article.h"
#include <boost/asio.hpp>
#include <boost/process.hpp>
#include <map>

namespace bp = boost::process;

class ArticleProcessor
{
public:
	ArticleProcessor(boost::asio::io_service* ios, size_t buff_max_size, const std::vector<std::string>& dict_words)
		:asio_service_(ios),
		buffer_max_size_(buff_max_size),
		dictionary_(dict_words)
	{}

	void scrap_and_process(const std::string& url)
	{
		std::string buffer_str('\0', buffer_max_size_);
		bp::async_pipe apipe(*asio_service_);
		bp::child c(bp::search_path("python"), std::string("scrap.py ") + url, bp::std_out > apipe);

		auto get_res = [this, &buffer_str](const boost::system::error_code &ec, std::size_t size)
					{
						if(!ec)
							process_article(buffer_str);
						else
							std::cout << ec.message() << std::endl;
					};
		boost::asio::async_read(apipe, boost::asio::buffer(&buffer_str[0], buffer_str.size()), get_res);
	}

private:
	void process_article(const std::string& article_str)
	{
		try
		{
			Article article(article_str, dictionary_);
		}
		catch(std::exception& ex)
		{
			std::cout << "error processing article: " << ex.what() << std::endl;
		}
	}

private:
	boost::asio::io_service *asio_service_;
	size_t buffer_max_size_;
	Dictionary dictionary_;
};

#endif

