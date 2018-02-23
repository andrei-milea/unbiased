#define BOOST_TEST_MODULE "test_vocabulary"

#include "../utils/tokenize.h"
#include "../vocabulary.h"
#include "../config.h"
#include "boost/test/included/unit_test.hpp"

using namespace boost::unit_test;
using namespace std;

const string words_filename("words.dat");
const string stopwords_filename("stop_words.dat");
const string vocab_path = Config::get().vocabulary_path + "test";

/*BOOST_AUTO_TEST_CASE(test_stemmer)
{
	std::ifstream words_file(words_filename);
	if(!words_file.is_open())
		throw std::runtime_error("Vocabulary::Vocabulary error: failed to open words file - " + words_filename);

	std::string words_str, stopwords_str;
	std::getline(words_file, words_str, '\n');
	
	std::vector<std::string> words = tokenize_stem(words_str);
	for(const auto& word : words)
	{
		cout << word << endl;
	}
}*/


BOOST_AUTO_TEST_CASE(test_save_load)
{
	Vocabulary vocab(words_filename, stopwords_filename);
	BOOST_REQUIRE(vocab.words_no() == 152588);
	BOOST_REQUIRE(vocab.stopwords_no() == 153);

	//save vocab
	{
		std::ofstream ofs(vocab_path);
		boost::archive::text_oarchive oarchive(ofs);
		oarchive << vocab;
	}

	Vocabulary saved_vocab;
	std::ifstream ifs(vocab_path);
	boost::archive::text_iarchive iarchive(ifs);
	iarchive >> saved_vocab;
	BOOST_REQUIRE(saved_vocab.words_no() == vocab.words_no());
}

BOOST_AUTO_TEST_CASE(test_stop_words)
{
	Vocabulary vocab(words_filename, stopwords_filename);
	BOOST_REQUIRE(vocab.words_no() == 152588);
	BOOST_REQUIRE(vocab.stopwords_no() == 153);
	BOOST_REQUIRE(vocab.is_stop_word("to"));
	const auto& stop_words = vocab.get_stop_words();
	WordInt word_id = 0;
	for(const auto& stop_word : stop_words)
		BOOST_REQUIRE(false == vocab.get_word_id(stop_word, word_id));
}

BOOST_AUTO_TEST_CASE(test_word_freq)
{
	Vocabulary vocab(words_filename, stopwords_filename);
	BOOST_REQUIRE(vocab.words_no() == 152588);
	BOOST_REQUIRE(vocab.stopwords_no() == 153);

	vocab.increase_word_freq("tabl");
	vocab.increase_word_freq("water");
	vocab.increase_word_freq("mous");

	BOOST_REQUIRE(vocab.get_word_freq("tabl") == 1);
	BOOST_REQUIRE(vocab.get_word_freq("water") == 1);
	BOOST_REQUIRE(vocab.get_word_freq("mous") == 1);
}

