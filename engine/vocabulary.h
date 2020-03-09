#ifndef _VOCABULARY_H
#define _VOCABULARY_H

#include "article.h"
#include "stemid_tokens_rcumap.h"
#include "utils/tokenize.h"
#include <atomic>
#include <boost/container_hash/hash.hpp>
#include <string>
#include <unordered_set>
#include <vector>

class BsonBuilder;

class Vocabulary
{
public:
    Vocabulary() = default;
    Vocabulary(const std::string& words_filename, const std::string& stopwords_filename);

    void increase_word_freq(WordId word_id) noexcept
    {
        assert(word_id < words_freq_.size());
        words_freq_[word_id].fetch_add(1, std::memory_order_relaxed);
    }

    //thread safe - atomic
    void increase_words_freq(const std::set<WordId>& word_ids) noexcept
    {
        for (WordId word_id : word_ids)
        {
            assert(word_id < words_freq_.size());
            words_freq_[word_id].fetch_add(1, std::memory_order_relaxed);
        }
    }

    int32_t get_word_freq(WordId word_id) const
    {
        assert(word_id < words_freq_.size());
        return words_freq_[word_id];
    }

    void add_words_measures(Article& article) const;

    bool get_word_stem(WordId idx, std::string& word) const noexcept
    {
        if (idx >= words_.size())
            return false;
        auto it = words_.begin();
        std::advance(it, idx);
        word = *it;
        return true;
    }

    bool get_word_id(const std::string& word, WordId& id) const noexcept
    {
        auto it = words_.find(word);
        if (it == words_.end())
            return false;
        id = std::distance(words_.begin(), it);
        return true;
    }

    bool is_stop_word(const std::string& word) const noexcept
    {
        auto it = stop_words_.find(word);
        if (it == stop_words_.end())
            return false;
        return true;
    }

    size_t words_no() const noexcept
    {
        return words_.size();
    }

    size_t stopwords_no() const noexcept
    {
        return stop_words_.size();
    }

    const std::unordered_set<std::string>& get_stop_words() const noexcept
    {
        return stop_words_;
    }

    const std::set<std::string>& get_words() const noexcept
    {
        return words_;
    }

    std::vector<uint64_t> get_words_freq() const
    {
        std::vector<uint64_t> res(words_freq_.begin(), words_freq_.end());
        return res;
    }

    void add_token(WordId word_id, const std::string& token)
    {
        stemid_tokens_map_.add_token(word_id, token);
    }

    std::shared_ptr<TokensSet> get_tokens(WordId id) const
    {
        return stemid_tokens_map_.get_tokens(id);
    }

private:
    std::string get_stem(const std::string& token) const
    {
        std::string res = token;
        Porter2Stemmer::trim(res);
        Porter2Stemmer::stem(res);
        return res;
    }

    //TODO fix 32-64bit combine
    uint32_t get_shingle(const std::string& w1, const std::string& w2, const std::string& w3) const
    {
        std::hash<std::string> hash_fct {};
        uint64_t hash_val = hash_fct(w1);
        uint32_t hash_val1 = hash_fct(w2);
        uint32_t hash_val2 = hash_fct(w3);
        boost::hash_combine(hash_val, hash_val1);
        boost::hash_combine(hash_val, hash_val2);
        return (uint32_t)hash_val;
    }

private:
    std::set<std::string> words_;
    std::unordered_set<std::string> stop_words_;
    std::array<std::atomic<int32_t>, MAX_WORDS_NO> words_freq_;
    StemIdTokensRCUMap stemid_tokens_map_;
    friend BsonBuilder;
};

#endif
