#ifndef _ENGINE_H
#define _ENGINE_H

#include "config.h"
#include "thread_pool.h"
#include "entry_processor.h"

class Engine
{
public:
	Engine()
		:entry_processor_(Config::get().scrapper_buff_size)
	{}

	EntryProcessor& get_processor()
	{
		return entry_processor_;
	}

private:
	EntryProcessor entry_processor_;
};

#endif

