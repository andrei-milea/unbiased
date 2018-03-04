#ifndef _VOCABULARY_H
#define _VOCABULARY_H

#include "utils/tokenize.h"
#include <iostream>
#include <string>
#include <vector>
#include <unordered_set>
#include <map>
#include <atomic>
#include <fstream>

#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/unordered_set.hpp>
#include <boost/serialization/map.hpp>
#include <boost/serialization/array.hpp>

typedef uint32_t WordInt;

template<typename Archive> 
void serialize(Archive & ar, std::atomic<uint64_t> atom, unsigned version)
{
}



// outside of any namespace
BOOST_SERIALIZATION_SPLIT_FREE(std::atomic<uint64_t>)

namespace boost { namespace serialization {
	template<class Archive>
	void save(Archive& ar, const std::atomic<uint64_t>& atom, unsigned int) {
		ar << atom.load(); 
	}
	template<class Archive>
	void load(Archive& ar, std::atomic<uint64_t>& atom, unsigned int) {
		uint64_t val;
		ar >> val; 
		atom = val;
	}
}} // namespace boost::serialization


class Vocabulary
{
public:
	Vocabulary()
	:new_words_idx_(0)
	{
	}

	Vocabulary(const std::string& words_filename, const std::string& stopwords_filename)
	:new_words_idx_(0)
	{
		std::ifstream words_file(words_filename);
		std::ifstream stopwords_file(stopwords_filename);
		if(!words_file.is_open())
			throw std::runtime_error("Vocabulary::Vocabulary error: failed to open words file - " + words_filename);
		if(!stopwords_file.is_open())
			throw std::runtime_error("Vocabulary::Vocabulary error: failed to open words file - " + stopwords_filename);

		std::string words_str, stopwords_str;
		std::getline(words_file, words_str, '\n');
		std::getline(stopwords_file, stopwords_str, '\n');

		
		std::set<std::string> words = tokenize_stem(words_str);
		std::vector<std::string> stopwords = tokenize(stopwords_str);

		for(const auto& stop_word : stopwords)
			stop_words_.insert(stop_word);

		size_t count = 0;
		for(const auto& word :  words)
		{
			if(stop_words_.find(word) == stop_words_.end())
			{
				words_map_[word] = count;
				words_freq_[count] = 0;
				count++;
			}
		}

	}

	friend class boost::serialization::access;
    template<class Archive>
    void serialize(Archive & ar, const unsigned int version)
    {
        ar & words_map_;
        ar & words_freq_;
        ar & stop_words_;
        ar & new_words_;
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

	//thread safe - atomic
	void increase_word_freq(WordInt word_id)
	{
		assert(word_id < words_freq_.size());
		words_freq_[word_id]++;
	}

	uint64_t get_word_freq(WordInt word_id)const
	{
		assert(word_id < words_freq_.size());
		return words_freq_[word_id];
	}

	bool get_word_id(const std::string& word, WordInt &id)const
	{
		auto it = words_map_.find(word);
		if(it == words_map_.end())
			return false;
		id = it->second;
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

	size_t stopwords_no()const
	{	return stop_words_.size();	}

	const std::unordered_set<std::string>& get_stop_words()const
	{
		return stop_words_;
	}


private:
	static constexpr size_t MAX_WORDS_NO = 300000;
	static constexpr size_t MAX_NEW_WORDS = 100;
	std::map<std::string, WordInt> words_map_;
	std::array<std::atomic<uint64_t>, MAX_WORDS_NO> words_freq_;
	std::unordered_set<std::string> stop_words_;
	std::array<std::string, MAX_NEW_WORDS> new_words_;
	std::atomic<size_t> new_words_idx_;
};


#endif

