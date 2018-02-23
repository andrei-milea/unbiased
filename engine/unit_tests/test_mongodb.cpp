#define BOOST_TEST_MODULE "test_mongodb"

#include "../mongodb.h"
#include "../config.h"
#include "boost/test/included/unit_test.hpp"

using namespace boost::unit_test;

BOOST_AUTO_TEST_CASE(test_database)
{
	const auto& mongo_inst = MongoDb::get();
	auto articles_no = mongo_inst.get_articles_no();
	BOOST_REQUIRE(articles_no == 0);
}
