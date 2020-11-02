#ifndef _VOCABULARY_H
#define _VOCABULARY_H

#include "stemid_tokens_rcumap.h"
#include "utils/tokenize.h"
#include <atomic>
#include <string>
#include <unordered_set>
#include <vector>

class Vocabulary
{
public:
    Vocabulary() = default;
    Vocabulary(const std::string& words_filename, const std::string& stopwords_filename);

    Vocabulary(const Vocabulary& vocab) = delete;
    Vocabulary(Vocabulary&& vocab) = delete;
    Vocabulary& operator=(const Vocabulary& vocab) = delete;

    void increase_stem_freq(StemId stem_id) const noexcept
    {
        assert(stem_id < stems_freq_.size());
        stems_freq_[stem_id].fetch_add(1, std::memory_order_relaxed);
    }

    //thread safe - atomic
    void increase_stems_freq(const std::set<StemId>& stems_ids) const noexcept
    {
        for (StemId stem_id : stems_ids)
        {
            assert(stem_id < stems_freq_.size());
            stems_freq_[stem_id].fetch_add(1, std::memory_order_relaxed);
        }
    }

    int32_t get_stem_freq(StemId stem_id) const
    {
        assert(stem_id < stems_freq_.size());
        return stems_freq_[stem_id];
    }

    bool get_stem(StemId idx, std::string& stem) const noexcept
    {
        if (idx >= stems_.size())
            return false;
        auto it = stems_.begin();
        std::advance(it, idx);
        stem = *it;
        return true;
    }

    bool get_stem_id(const std::string& stem, StemId& id) const noexcept
    {
        auto it = stems_.find(stem);
        if (it == stems_.end())
            return false;
        id = std::distance(stems_.begin(), it);
        return true;
    }

    bool is_valid_stem(const std::string& stem) const noexcept
    {
        return stems_.find(stem) != stems_.end();
    }

    bool is_stop_word(const std::string& word) const noexcept
    {
        return stop_words_.find(word) != stop_words_.end();
    }

    size_t stems_no() const noexcept
    {
        return stems_.size();
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

    std::vector<uint64_t> get_stems_freq() const
    {
        std::vector<uint64_t> res(stems_freq_.begin(), stems_freq_.end());
        return res;
    }

    void add_stems(std::set<std::string>&& stems)
    {
        stems_.merge(stems);
    }

    void add_token(StemId stem_id, const std::string& token) const
    {
        stemid_tokens_map_.add_token(stem_id, token);
    }

    std::shared_ptr<TokensSet> get_tokens(StemId id) const
    {
        return stemid_tokens_map_.get_tokens(id);
    }

    const std::set<std::string>& get_stems() const noexcept
    {
        return stems_;
    }

    bool is_valid_word(const std::string& word) const noexcept
    {
        return words_.count(word);
    }

private:
    std::set<std::string> stems_;
    std::unordered_set<std::string> words_;
    std::unordered_set<std::string> stop_words_;
    mutable std::array<std::atomic<int32_t>, MAX_STEMS_NO> stems_freq_;
    mutable StemIdTokensRCUMap stemid_tokens_map_;
};

#endif
