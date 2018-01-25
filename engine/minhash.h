#ifndef _MINHASH_H
#define _MINHASH_H

#include "utils/murmur3.h"
#include <cstdint>
#include <set>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <cassert>
#include <boost/functional/hash.hpp>


class MinHash
{
public:
    MinHash(size_t hash_fcts_no, int bands_no, size_t range_max)
        :hash_fcts_no_(hash_fcts_no),
		bands_no_(bands_no),
        range_max_(range_max)
    {
        min_hash_values_.resize(hash_fcts_no_);
    }

    ~MinHash()  {}

	Signature compute_signature(const std::set<uint32_t> &shingles)const
    {
		Signature signature;
        for(const auto &elem : shingles)
        {
            for(size_t hidx = 0; hidx < hash_fcts_no_; hidx++)
            {
                uint32_t hash_val = 0;
                MurmurHash3_x86_32(&elem, sizeof(uint32_t), hidx, &hash_val);
                hash_val %= range_max_;
                if(min_hash_values_[hidx] > hash_val)
                    min_hash_values_[hidx] = hash_val;
            }
        }
    }

	inline std::vector<std::unordered_map<size_t, std::vector<size_t>>> compute_lsh_groups(size_t bands_no, size_t hash_fcts_no, const std::vector<std::vector<uint64_t>>& signatures)
    {
        assert(bands_no < hash_fcts_no);
        std::vector<std::unordered_map<size_t, std::vector<size_t>>> lsh_buckets;
        lsh_buckets.resize(bands_no);
        size_t rows_no = hash_fcts_no / bands_no;
        for(size_t sidx = 0; sidx < signatures.size(); sidx++)
        {
            size_t band_rows = 0, row_idx = 0;
            for(size_t band_idx = 0; band_idx < bands_no; band_idx++)
            {
                size_t hash_value = 17;
                band_rows += rows_no;
                for(; row_idx < band_rows; row_idx++)
                {
                    boost::hash_combine(hash_value, signatures[sidx][row_idx]);
                }
                lsh_buckets[band_idx][hash_value].push_back(sidx);
            }
        }
        return lsh_buckets;
    }

private:
    size_t hash_fcts_no_;
	int bands_no_;
    size_t range_max_;
	mutable std::vector<uint32_t> min_hash_values_;
};

#endif
