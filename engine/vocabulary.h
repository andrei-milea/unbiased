#ifndef _VOCABULARY_H
#define _VOCABULARY_H

#include "utils/tokenize.h"
#include "article.h"
#include <iostream>
#include <string>
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <atomic>
#include <fstream>

class BsonBuilder;

class Vocabulary
{
public:
	Vocabulary(){};
	Vocabulary(const std::string& words_filename, const std::string& stopwords_filename);

	void increase_word_freq(size_t word_id)noexcept
	{
		assert(word_id < words_freq_.size());
		words_freq_[word_id]++;

	}

	//thread safe - atomic
	void increase_words_freq(const std::set<size_t> &word_ids)noexcept
	{
		for(size_t word_id : word_ids)
		{
			assert(word_id < words_freq_.size());
			words_freq_[word_id]++;
		}
	}

	uint64_t get_word_freq(size_t word_id)const
	{
		assert(word_id < words_freq_.size());
		return words_freq_[word_id];
	}

	void add_words_measures(Article& article)const;

	bool get_word(size_t idx, std::string& word)const noexcept
	{
		if(idx >= words_.size())
			return false;
		auto it = words_.begin();
		std::advance(it, idx);
		word = *it;
		return true;
	}

	bool get_word_id(const std::string& word, size_t &id)const noexcept
	{
		auto it = words_.find(word);
		if(it == words_.end())
			return false;
		id = std::distance(words_.begin(), it);
		return true;
	}

	bool is_stop_word(const std::string& word)const noexcept
	{
		auto it = stop_words_.find(word);
		if(it == stop_words_.end())
			return false;
		return true;
	}

	size_t words_no()const noexcept
	{	return words_.size();	}

	size_t stopwords_no()const noexcept
	{	return stop_words_.size();	}

	const std::unordered_set<std::string>& get_stop_words()const noexcept
	{
		return stop_words_;
	}

	const std::set<std::string>& get_words()const noexcept
	{
		return words_;
	}

	std::vector<uint64_t> get_words_freq()const
	{
		std::vector<uint64_t> res(words_freq_.begin(), words_freq_.end());
		return res;
	}

private:
	std::string get_stem(const std::string& token)const
	{
		std::string res = token;
		Porter2Stemmer::trim(res);
		Porter2Stemmer::stem(res);
		return res;
	}

	//TODO fix 32-64bit combine
	uint32_t get_shingle(const std::string& w1, const std::string& w2, const std::string& w3)const
	{
		std::hash<std::string> hash_fct{};
        uint64_t hash_val = hash_fct(w1);
        uint32_t hash_val1 = hash_fct(w2);
        uint32_t hash_val2 = hash_fct(w3);
        boost::hash_combine(hash_val, hash_val1);
        boost::hash_combine(hash_val, hash_val2);
		return (uint32_t)hash_val;
	}

private:
	static constexpr size_t MAX_WORDS_NO = 300000;
	std::set<std::string> words_;
	std::array<std::atomic<uint64_t>, MAX_WORDS_NO> words_freq_;
	std::unordered_set<std::string> stop_words_;
	friend BsonBuilder;
};


#endif

