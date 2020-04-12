#define BOOST_TEST_MODULE "test_vocabulary"

#include "../utils/stl_ext.h"
#include "../utils/tokenize.h"
#include "../utils/log_helper.h"
#include "../vocabulary.h"
#include "boost/test/included/unit_test.hpp"
#include <thread>

using namespace boost::unit_test;
using namespace std;

const string words_filename{"words.dat"};
const string stopwords_filename{"stop_words.dat"};
RegisterUnitTestLogger register_logger{BOOST_TEST_MODULE};


BOOST_AUTO_TEST_CASE(test_tokens_rcu_map)
{
    Vocabulary vocab { words_filename, stopwords_filename };
    BOOST_REQUIRE(vocab.stems_no() == 152588);

    StemIdTokensRCUMap tokens_rcu_map;

    std::ifstream words_file{words_filename};
    if (!words_file.is_open())
        throw std::runtime_error("Vocabulary::Vocabulary error: failed to open words file - " + words_filename);

    string words_str, stopwords_str;
    getline(words_file, words_str, '\n');

    vector<std::string> words = tokenize(words_str);

    map<StemId, TokensSet> ids_words_map;

    auto add_tokens_lmbd = [&](bool add) {
        for (const auto& word : words)
        {
            std::string word_stem = word;

            Porter2Stemmer::trim(word_stem);
            Porter2Stemmer::stem(word_stem);

            StemId stem_id;
            vocab.get_stem_id(word_stem, stem_id);

            tokens_rcu_map.add_token(stem_id, word);
            if (add)
                ids_words_map[stem_id].insert(word);
        }
    };

    int32_t count = 0;
    auto read_tokens_lmbd = [&]() {
        for (const auto& word : words)
        {
            std::string word_stem = word;

            Porter2Stemmer::trim(word_stem);
            Porter2Stemmer::stem(word_stem);

            StemId stem_id;
            vocab.get_stem_id(word_stem, stem_id);
            auto res = tokens_rcu_map.get_tokens(stem_id);
            if (res)
                count += res->size();
        }
    };

    thread thread_write1(add_tokens_lmbd, true);
    thread thread_write2(add_tokens_lmbd, false);
    thread thread_read(read_tokens_lmbd);

    thread_write1.join();
    thread_write2.join();
    thread_read.join();
    spdlog::info("tokens count {}", count);
    spdlog::info("ids_words_map size {}", ids_words_map.size());
    for (const auto elem : ids_words_map)
    {
        auto ids_set = tokens_rcu_map.get_tokens(elem.first);
        BOOST_REQUIRE_EQUAL(*ids_set, elem.second);
    }
}

BOOST_AUTO_TEST_CASE(test_stop_words)
{
    Vocabulary vocab { words_filename, stopwords_filename };
    BOOST_REQUIRE(vocab.stems_no() == 152588);
    BOOST_REQUIRE(vocab.stopwords_no() == 153);
    BOOST_REQUIRE(vocab.is_stop_word("to"));
    const auto& stop_words = vocab.get_stop_words();
    StemId stem_id = 0;
    for (const auto& stop_word : stop_words)
        BOOST_REQUIRE(false == vocab.get_stem_id(stop_word, stem_id));
}

BOOST_AUTO_TEST_CASE(test_word_freq)
{
    Vocabulary vocab { words_filename, stopwords_filename };
    BOOST_REQUIRE(vocab.stems_no() == 152588);
    BOOST_REQUIRE(vocab.stopwords_no() == 153);

    StemId id1 = 0, id2 = 0, id3 = 0;
    bool res = vocab.get_stem_id("tabl", id1);
    BOOST_REQUIRE(res);
    vocab.increase_stem_freq(id1);

    res = vocab.get_stem_id("water", id2);
    BOOST_REQUIRE(res);
    vocab.increase_stem_freq(id2);

    res = vocab.get_stem_id("mous", id3);
    BOOST_REQUIRE(res);
    vocab.increase_stem_freq(id3);

    BOOST_REQUIRE(vocab.get_stem_freq(id1) == 1);
    BOOST_REQUIRE(vocab.get_stem_freq(id2) == 1);
    BOOST_REQUIRE(vocab.get_stem_freq(id3) == 1);
}

