#include "config.h"
#include "db.h"
#include "thread_pool.h"
#include "article_processor.h"
#include <csignal>
#include <exception>

using namespace std;

boost::asio::io_service io_service;
ThreadPool thread_pool(&io_service, Config::get().threads_no);
ArticleProcessor article_processor(&io_service, Config::get().scrapper_buff_size);

void signal_handler(int sig)
{
	cout << "handler called\n";
	const std::string connection_params = std::string("host=") + Config::get().host + " user=" + Config::get().username +
											" password=" + Config::get().password + " dbname=" + Config::get().dbname;
	const std::string select_entries_str("SELECT * FROM backend_userentry WHERE processed = False");
	try
	{
		DbAccess db(connection_params);
		auto user_entries = db.execute(select_entries_str);
		for(const auto& user_entry : user_entries)
		{
			assert(user_entry.size() == 5);
			article_processor.scrap_and_process(user_entry[1]);
		}
		//thread_pool.post();
		
		/*for(const auto& user_entry : user_entries)
		{
			cout << "entry: ";
			for(const auto& entry_elem : user_entry)
				cout << entry_elem << " ";
			cout << endl;
		}*/
	}
	catch(const std::exception& ex)
	{
		cout << "signal_handler failed: " << ex.what() << endl;
	}
}

int main()
{
	//register signal from django
	signal(SIGUSR1, signal_handler);	
	signal_handler(SIGUSR1);
	while(true)
	{
	}
	return 0;
}
