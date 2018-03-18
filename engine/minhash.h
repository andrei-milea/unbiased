#ifndef _MINHASH_H
#define _MINHASH_H

#include "utils/murmur3.h"
#include "article.h"
#include <cstdint>
#include <set>

class MinHash
{
public:
    MinHash(size_t hash_fcts_no, size_t range_max)
	:hash_fcts_no_(hash_fcts_no),
	range_max_(range_max)
    {
    }

    ~MinHash()  {}

	Signature compute_signature(const std::set<uint32_t> &shingles)const
    {
		Signature signature;
		for(size_t idx = 0; idx < signature.size(); idx++)
			signature[idx] = range_max_ + 1;
        for(const auto &elem : shingles)
        {
            for(size_t hidx = 0; hidx < hash_fcts_no_; hidx++)
            {
                uint32_t hash_val = 0;
                MurmurHash3_x86_32(&elem, sizeof(uint32_t), hidx, &hash_val);
                hash_val %= range_max_;
                if(signature[hidx] > hash_val)
                    signature[hidx] = hash_val;
            }
        }
		return signature;
    }

private:
    size_t hash_fcts_no_;
    size_t range_max_;
};

#endif
