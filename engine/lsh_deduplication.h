#ifndef _LSH_DEDUPLICATION_H
#define _LSH_DEDUPLICATION_H

#include "article.h"
#include <boost/functional/hash.hpp>
#include <unordered_map>
#include <vector>
#include <cassert>
#include <iostream>

class LSHDeduplication
{
public:
	LSHDeduplication(size_t signature_size, const std::vector<std::pair<std::string, Signature>>& docs_signatures)
	:signature_size_(signature_size),
	bands_no_(20),
	rows_no_(signature_size_ / bands_no_)
	{
        lsh_buckets_.resize(bands_no_);
		compute_lsh_groups(docs_signatures);
	}

	std::set<std::string> process_doc(const std::pair<std::string, Signature>& doc_signature)
	{
		std::set<std::string> duplicates;
		duplicates.insert(doc_signature.first);
		size_t band_rows = 0, row_idx = 0;
		for(size_t band_idx = 0; band_idx < bands_no_; band_idx++)
		{
			size_t hash_value = 17;
			band_rows += rows_no_;
			for(; row_idx < band_rows; row_idx++)
			{
				boost::hash_combine(hash_value, doc_signature.second[row_idx]);
			}
			if(lsh_buckets_[band_idx].find(hash_value) != lsh_buckets_[band_idx].end())
				duplicates.insert(lsh_buckets_[band_idx][hash_value].begin(), lsh_buckets_[band_idx][hash_value].end());
			lsh_buckets_[band_idx][hash_value].insert(doc_signature.first);
		}
		return duplicates;
	}

private:
	void compute_lsh_groups(const std::vector<std::pair<std::string, Signature>>& docs_signatures)
    {
        assert(bands_no_ < signature_size_);
        for(size_t sidx = 0; sidx < docs_signatures.size(); sidx++)
        {
            size_t band_rows = 0, row_idx = 0;
            for(size_t band_idx = 0; band_idx < bands_no_; band_idx++)
            {
                size_t hash_value = 17;
                band_rows += rows_no_;
                for(; row_idx < band_rows; row_idx++)
                {
                    boost::hash_combine(hash_value, docs_signatures[sidx].second[row_idx]);
                }
                lsh_buckets_[band_idx][hash_value].insert(docs_signatures[sidx].first);
            }
        }
    }

	

private:
	size_t signature_size_;
	size_t bands_no_;
	size_t rows_no_;
	std::vector<std::unordered_map<size_t, std::set<std::string>>> lsh_buckets_;
};

#endif

