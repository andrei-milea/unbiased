#ifndef _ENTRY_PROCESSOR_H
#define _ENTRY_PROCESSOR_H

#include "mongodb.h"
#include "vocabulary.h"
#include "article.h"
#include "article_builder.h"
#include <boost/asio.hpp>
#include <boost/process.hpp>
#include <map>

namespace bp = boost::process;

class EntryProcessor
{
public:
	EntryProcessor(boost::asio::io_service* ios, size_t buff_max_size)
		:asio_service_(ios),
		buffer_max_size_(buff_max_size),
		vocabulary_(MongoDb::get().load_vocabulary_map(), MongoDb::get().load_vocabulary_stop_words()),
		article_builder_(vocabulary_, MongoDb::get().get_articles_no(), MongoDb::get().load_articles_signatures())
	{
	}

	void scrap_and_process(const std::string& url)const
	{
		std::string buffer_str('\0', buffer_max_size_);
		bp::async_pipe apipe(*asio_service_);
		bp::child c(bp::search_path("python"), std::string("scrap.py ") + url, bp::std_out > apipe);

		auto get_res = [this, &buffer_str](const boost::system::error_code &ec, std::size_t size)
					{
						if(!ec)
							process_entry(buffer_str);
						else
							std::cout << ec.message() << std::endl;
					};
		boost::asio::async_read(apipe, boost::asio::buffer(&buffer_str[0], buffer_str.size()), get_res);
	}

private:
	void process_entry(const std::string& entry_str)const
	{
		try
		{
			auto article = article_builder_.from_xml(entry_str);
			MongoDb::get().save_article(article);
		}
		catch(std::exception& ex)
		{
			std::cout << "error processing article: " << ex.what() << std::endl;
		}
	}

private:
	boost::asio::io_service *asio_service_;
	size_t buffer_max_size_;
	Vocabulary vocabulary_;
	ArticleBuilder article_builder_;
};

#endif

