#ifndef _DICTIONARY_H
#define _DICTIONARY_H

#include "types.h"
#include <string>
#include <vector>
#include <unordered_map>

class Dictionary
{
public:
	Dictionary(const std::vector<std::string>& words)
		:words_(words),
		new_words_idx_(0)
	{
		std::sort(words_.begin(), words_.end());
		for(size_t idx = 0; idx < words_.size(); idx++)
			words_ids_[words_[idx]] = (WordInt)idx;
	}

	void add_word(const std::string& word)
	{
		if(new_words_idx_ < MAX_NEW_WORDS)
		{
			new_words_idx_++;
			new_words_[new_words_idx_] = (word);
		}
		else
			std::cout << "too many new words added\n";
	}

	bool get_word_id(const std::string& word, WordInt &id)const
	{
		auto it = words_ids_.find(word);
		if(it == words_ids_.end())
			return false;
		id = it->second;
		return true;
	}

	const std::string& get_word(WordInt id)const
	{
		return words_[id];
	}

	//no other operations should take place while this method is called
	void update_dictionary()
	{
		words_.insert(words_.end(), new_words_.begin(), new_words_.end());
		new_words_idx_ = 0;
		std::sort(words_.begin(), words_.end());
		for(size_t idx = 0; idx < words_.size(); idx++)
			words_ids_[words_[idx]] = idx;
	}

	size_t size()const
	{	return words_.size();	}

private:
	static constexpr size_t MAX_NEW_WORDS = 100;
	std::vector<std::string> words_;
	std::unordered_map<std::string, WordInt> words_ids_;
	std::array<std::string, MAX_NEW_WORDS> new_words_;
	std::atomic<size_t> new_words_idx_;
};


#endif

