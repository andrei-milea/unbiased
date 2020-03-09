#include "engine.h"
#include "lsa.h"
#include "mongodb.h"
#include "pqldb.h"
#include "utils/article_utils.h"
#include <csignal>
#include <exception>

using namespace std;

int main()
{
    Engine engine;
    engine.get_processor().run(3);

    try
    {
        //TODO use zeromq
        /*while(true)
		{
			auto message = receiver.fetch();
			std::cout << message.getContent() << std::endl;
			engine.get_processor().scrap_and_process(message.getContent());
			session.acknowledge();
		}*/
    }
    catch (const std::exception& ex)
    {
        cout << "critical error" << ex.what() << "\n";
    }
    return 0;
}
