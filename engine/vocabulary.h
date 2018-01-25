#ifndef _VOCABULARY_H
#define _VOCABULARY_H

#include <iostream>
#include <string>
#include <vector>
#include <unordered_set>
#include <map>
#include <atomic>


typedef uint32_t WordInt;
struct WordInfo
{
	WordInt word_id;
	uint64_t freq;
};

class Vocabulary
{
public:
	Vocabulary(const std::map<std::string, WordInfo>& words_map, const std::unordered_set<std::string>& stop_words)
		:words_map_(words_map),
		stop_words_(stop_words),
		new_words_idx_(0)
	{
	}

	void add_new_word(const std::string& word)
	{
		if(new_words_idx_ < MAX_NEW_WORDS)
		{
			new_words_idx_++;
			new_words_[new_words_idx_] = (word);
		}
		else
			std::cout << "too many new words added\n";
	}

	void increase_word_freq(const std::string& word)
	{
		words_map_.at(word).freq++;
	}

	uint64_t get_word_freq(const std::string& word)const
	{
		return words_map_.at(word).freq;
	}

	bool get_word_id(const std::string& word, WordInt &id)const
	{
		auto it = words_map_.find(word);
		if(it == words_map_.end())
			return false;
		id = it->second.word_id;
		return true;
	}

	bool is_stop_word(const std::string& word)const
	{
		auto it = stop_words_.find(word);
		if(it == stop_words_.end())
			return false;
		return true;
	}

	//no other operations should take place while this method is called
	void synchronize_dictionary()
	{
	}

	size_t words_no()const
	{	return words_map_.size();	}

private:
	static constexpr size_t MAX_NEW_WORDS = 100;
	std::map<std::string, WordInfo> words_map_;
	std::unordered_set<std::string> stop_words_;
	std::array<std::string, MAX_NEW_WORDS> new_words_;
	std::atomic<size_t> new_words_idx_;
};


#endif

