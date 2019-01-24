#include "engine.h"
#include "pqldb.h"
#include "mongodb.h"
#include "lsa.h"
#include "utils/article_utils.h"
#include <csignal>
#include <exception>

#include <qpid/messaging/Connection.h>
#include <qpid/messaging/Message.h>
#include <qpid/messaging/Receiver.h>
#include <qpid/messaging/Sender.h>
#include <qpid/messaging/Session.h>

using namespace std;
using namespace qpid::messaging;


int main()
{
	Engine engine;
	engine.get_processor().run(3);

	try
	{
		//TODO - read these from config.xml
		const string broker = "localhost:5673";
		const string address = "unbiased.new_urls; {create: always, node: {type: 'queue'}}";
		const string connection_options = "";
		Connection connection(broker, connection_options);
		connection.open();
		Session session = connection.createSession();
		Receiver receiver = session.createReceiver(address);
		while(true)
		{
			auto message = receiver.fetch();
			std::cout << message.getContent() << std::endl;
			engine.get_processor().scrap_and_process(message.getContent());
			session.acknowledge();
		}
	}
	catch(const std::exception& ex)
	{
		cout << "critical error" << ex.what() << "\n";
	}
	return 0;
}
