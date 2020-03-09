#define BOOST_TEST_MODULE "test_vocabulary"

#include "../config.h"
#include "../utils/stl_ext.h"
#include "../utils/tokenize.h"
#include "../vocabulary.h"
#include "boost/test/included/unit_test.hpp"
#include <thread>

using namespace boost::unit_test;
using namespace std;

const string words_filename("words.dat");
const string stopwords_filename("stop_words.dat");

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
BOOST_AUTO_TEST_CASE(test_tokens_rcu_map)
{
    Vocabulary vocab { words_filename, stopwords_filename };
    BOOST_REQUIRE(vocab.words_no() == 152588);

    StemIdTokensRCUMap tokens_rcu_map;

    std::ifstream words_file(words_filename);
    if (!words_file.is_open())
        throw std::runtime_error("Vocabulary::Vocabulary error: failed to open words file - " + words_filename);

    string words_str, stopwords_str;
    getline(words_file, words_str, '\n');

    vector<std::string> words = tokenize(words_str);

    map<WordId, TokensSet> ids_words_map;

    auto add_tokens_lmbd = [&](bool add) {
        for (const auto& word : words)
        {
            std::string word_stem = word;

            Porter2Stemmer::trim(word_stem);
            Porter2Stemmer::stem(word_stem);

            WordId word_id;
            vocab.get_word_id(word_stem, word_id);

            tokens_rcu_map.add_token(word_id, word);
            if (add)
                ids_words_map[word_id].insert(word);
        }
    };

    auto read_tokens_lmbd = [&]() {
        for (const auto& word : words)
        {
            std::string word_stem = word;

            Porter2Stemmer::trim(word_stem);
            Porter2Stemmer::stem(word_stem);

            WordId word_id;
            vocab.get_word_id(word_stem, word_id);
            auto res = tokens_rcu_map.get_tokens(word_id);
            if (res)
                cout << res->size() << "\n";
        }
    };

    thread thread_write1(add_tokens_lmbd, true);
    thread thread_write2(add_tokens_lmbd, false);
    thread thread_read(read_tokens_lmbd);

    thread_write1.join();
    thread_write2.join();
    thread_read.join();

    cout << "map size: " << ids_words_map.size() << endl;
    for (const auto elem : ids_words_map)
    {
        auto ids_set = tokens_rcu_map.get_tokens(elem.first);
        BOOST_REQUIRE_EQUAL(*ids_set, elem.second);

        std::string stem;
        bool res = vocab.get_word_stem(elem.first, stem);
        if (res)
            cout << "STEM: " << stem << "WORDS: " << elem.second << endl;
        else
            cout << "did not find word in vocabulary\n";
    }
}

BOOST_AUTO_TEST_CASE(test_stop_words)
{
    Vocabulary vocab { words_filename, stopwords_filename };
    BOOST_REQUIRE(vocab.words_no() == 152588);
    BOOST_REQUIRE(vocab.stopwords_no() == 153);
    BOOST_REQUIRE(vocab.is_stop_word("to"));
    const auto& stop_words = vocab.get_stop_words();
    WordId word_id = 0;
    for (const auto& stop_word : stop_words)
        BOOST_REQUIRE(false == vocab.get_word_id(stop_word, word_id));
}

BOOST_AUTO_TEST_CASE(test_word_freq)
{
    Vocabulary vocab { words_filename, stopwords_filename };
    BOOST_REQUIRE(vocab.words_no() == 152588);
    BOOST_REQUIRE(vocab.stopwords_no() == 153);

    WordId id1 = 0, id2 = 0, id3 = 0;
    bool res = vocab.get_word_id("tabl", id1);
    BOOST_REQUIRE(res);
    vocab.increase_word_freq(id1);

    res = vocab.get_word_id("water", id2);
    BOOST_REQUIRE(res);
    vocab.increase_word_freq(id2);

    res = vocab.get_word_id("mous", id3);
    BOOST_REQUIRE(res);
    vocab.increase_word_freq(id3);

    BOOST_REQUIRE(vocab.get_word_freq(id1) == 1);
    BOOST_REQUIRE(vocab.get_word_freq(id2) == 1);
    BOOST_REQUIRE(vocab.get_word_freq(id3) == 1);
}
