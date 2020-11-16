#ifndef _MINHASH_H
#define _MINHASH_H

#include "config.h"
#include "murmur3.h"
#include <cstdint>
#include <set>
#include <array>

constexpr int32_t MINHASH_RANGE_MAX = 100000000;


//class that computes the minhash signature
//for a given set of shingles
class MinHash
{
public:
    MinHash(int32_t hash_fcts_no, int32_t range_max)
        : hash_fcts_no_(hash_fcts_no)
        , range_max_(range_max)
    {
    }

    ~MinHash() {}

    //hash each shingle with a number of hash functions and store the minimum hash 
    //P(sign(A) == sign(B)) = Jacc(A, B) = (A intersect B) / (A U B)
    Signature compute_signature(const std::set<int32_t>& shingles) const
    {
        Signature signature;
        for (size_t idx = 0; idx < signature.size(); idx++)
            signature[idx] = range_max_ + 1;
        for (const auto& elem : shingles)
        {
            for (size_t hidx = 0; hidx < hash_fcts_no_; hidx++)
            {
                int32_t hash_val = 0;
                MurmurHash3_x86_32(&elem, sizeof(int32_t), hidx, &hash_val);
                hash_val %= range_max_;
                if (signature[hidx] > hash_val)
                    signature[hidx] = hash_val;
            }
        }
        return signature;
    }

private:
    int32_t hash_fcts_no_;
    int32_t range_max_;
};

#endif
