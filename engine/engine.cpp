#include "pqldb.h"
#include <csignal>
#include <exception>

using namespace std;
Engine g_engine;

void signal_handler(int sig)
{
	cout << "handler called\n";
	const std::string select_entries_str{"SELECT * FROM backend_userentry WHERE processed = False"};
	try
	{
		PqlDb db{connection_params};
		auto user_entries = PqlDb::get().get_unprocessed_entries();
		for(const auto& user_entry : user_entries)
		{
			assert(user_entry.size() == 5);
			g_engine.get_processor().scrap_and_process(user_entry[1]);
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
