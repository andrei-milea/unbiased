#ifndef _ENGINE_H
#define _ENGINE_H

#include "config.h"
#include "thread_pool.h"
#include "entry_processor.h"

class Engine
{
public:
	Engine()
		:thread_pool_(&io_service, Config::get().threads_no),
		entry_processor_(&io_service, Config::get().scrapper_buff_size)
	{}

	const get_processor()const
	{
		return entry_processor_;
	}

private:
	boost::asio::io_service io_service_;
	EntryProcessor entry_processor_;
};

#endif

