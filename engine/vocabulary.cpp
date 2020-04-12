#include "vocabulary.h"

#include <fstream>
#include <iostream>

using namespace std;

Vocabulary::Vocabulary(const string& words_filename, const string& stopwords_filename)
{

    ifstream words_file { words_filename };
    ifstream stopwords_file { stopwords_filename };
    if (!words_file.is_open())
        throw std::runtime_error("Vocabulary::Vocabulary error: failed to open words file - " + words_filename);
    if (!stopwords_file.is_open())
        throw std::runtime_error("Vocabulary::Vocabulary error: failed to open words file - " + stopwords_filename);

    string words_str, stopwords_str;
    getline(words_file, words_str, '\n');
    getline(stopwords_file, stopwords_str, '\n');

    set<string> stems = tokenize_stem(words_str);
    vector<string> stopwords = tokenize(stopwords_str);

    for (const auto& stop_word : stopwords)
        stop_words_.insert(stop_word);

    int32_t count = 0;
    for (const auto& stem : stems)
    {
        if (stop_words_.find(stem) == stop_words_.end())
        {
            stems_.insert(stem);
            stems_freq_[count] = 0;
            count++;
        }
    }
}

