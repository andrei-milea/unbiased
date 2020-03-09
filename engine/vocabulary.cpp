#include "vocabulary.h"

#include <iostream>
#include <fstream>

using namespace std;

Vocabulary::Vocabulary(const string& words_filename, const string& stopwords_filename)
{
	ifstream words_file{words_filename};
	ifstream stopwords_file{stopwords_filename};
	if(!words_file.is_open())
		throw std::runtime_error("Vocabulary::Vocabulary error: failed to open words file - " + words_filename);
	if(!stopwords_file.is_open())
		throw std::runtime_error("Vocabulary::Vocabulary error: failed to open words file - " + stopwords_filename);

	string words_str, stopwords_str;
	getline(words_file, words_str, '\n');
	getline(stopwords_file, stopwords_str, '\n');
	
	set<string> words = tokenize_stem(words_str);
	vector<string> stopwords = tokenize(stopwords_str);

	for(const auto& stop_word : stopwords)
		stop_words_.insert(stop_word);

	size_t count = 0;
	for(const auto& word :  words)
	{
		if(stop_words_.find(word) == stop_words_.end())
		{
			words_.insert(word);
			words_freq_[count] = 0;
			count++;
		}
	}
        cout << "stems added: " << count << endl;
}

void Vocabulary::add_words_measures(Article& article)const
{
	article.tf.resize(words_no(), 0.0);
	for(size_t tidx = 0; tidx < article.tokens.size(); tidx++)
	{
		if(true == is_stop_word(article.tokens[tidx])) //stop word detected
		{
			if(tidx + 2 < article.tokens.size())
			{
				auto shingle = get_shingle(article.tokens[tidx], article.tokens[tidx + 1], article.tokens[tidx + 2]);
				article.shingles.insert(shingle);
			}
		}
		else
		{
			article.words_no++;
			auto stem = get_stem(article.tokens[tidx]);
			WordId word_id = 0;
			if(true == get_word_id(stem, word_id))
			{
				assert(word_id < words_no());
				article.tf[word_id]++;
				article.ids_tokens_map[word_id] = tidx;
			}
			else if(false/* == is_misspelling(article.tokens[tidx])*/)
			{
				//deal with misspelings
			}
			else
			{
				article.unknown_words_no++;
			}
		}
	}
}

