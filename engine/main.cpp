#include "config.h"
#include "db.h"
#include <csignal>
#include <exception>

using namespace std;

void signal_handler(int sig)
{
	cout << "handler called\n";
	const std::string connection_params = std::string("host=") + Config::get().host + " user=" + Config::get().username +
											" password=" + Config::get().password + " dbname=" + Config::get().dbname;
	const std::string select_entries_str("SELECT * FROM backend_userentry WHERE processed = False");
	try
	{
		DbAccess db(connection_params);
		db.execute(select_entries_str);
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
