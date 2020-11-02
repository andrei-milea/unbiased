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

    vector<string> stopwords = tokenize(stopwords_str);
    for (const auto& stop_word : stopwords)
        stop_words_.insert(stop_word);

    vector<string> words = tokenize(words_str);
    words_.insert(words.begin(), words.end());
}

