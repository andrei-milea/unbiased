#ifndef _SIMILARITY_H
#define _SIMILARITY_H

#include <vector>
#include <set>
#include <unordered_map>
#include <boost/functional/hash.hpp>

namespace similarity
{

	template <typename T>
	inline float jaccard_index_bf(const std::set<T>& set1, const std::set<T>& set2)
	{
		std::vector<int> intersection;
		std::vector<int> set_union;
		std::set_intersection(set1.begin(), set1.end(), set2.begin(), set2.end(), std::back_inserter(intersection)); 
		std::set_union(set1.begin(), set1.end(), set2.begin(), set2.end(), std::back_inserter(set_union)); 
		return float(intersection.size()) / float(set_union.size());
	}

	inline float jaccard_index_sign(const std::vector<uint64_t>& sign_set1, const std::vector<uint64_t>& sign_set2, size_t hash_fct_no)
	{
		size_t count = 0;
		for(size_t hidx = 0; hidx < hash_fct_no; hidx++)
		{
			if(sign_set1[hidx] == sign_set1[hidx])
				count++;
		}
		return float(count) / float(hash_fct_no);
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

};

#endif

