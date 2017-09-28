#ifndef _THREAD_POOL_H
#define _THREAD_POOL_H

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/thread/thread.hpp>

class ThreadPool
{
public:
	ThreadPool(boost::asio::io_service* ios, size_t threads_no)
		:io_service_(ios),
		threads_no_(threads_no),
		work_(*io_service_)
	{
		for(size_t idx = 0; idx < threads_no_; idx++)
			threads_.create_thread(boost::bind(&boost::asio::io_service::run, io_service_));
	}

	template <typename T>
	void post_fct(T &fnc)
	{
		io_service_->post(fnc);
	}

	~ThreadPool()
	{
		io_service_->stop();
		threads_.join_all();
	}

private:
	boost::asio::io_service *io_service_;
	size_t threads_no_;
	boost::thread_group threads_;
	boost::asio::io_service::work work_;
};

#endif

