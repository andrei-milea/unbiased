#define BOOST_TEST_MODULE "test_min_hash"

#include "../utils/log_helper.h"
#include "../minhash.h"
#include "boost/test/included/unit_test.hpp"
#include <random>
#include <set>
#include <vector>

RegisterUnitTestLogger register_logger(BOOST_TEST_MODULE);

using namespace boost::unit_test;
using namespace std;

//generate a random set with elements in a given range
set<int32_t> generate_set(size_t size, const std::pair<size_t, size_t>& range)
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(range.first, range.second);
    set<int32_t> gen_set;
    for (size_t idx = 0; idx < size; idx++)
        gen_set.insert(dis(gen));
    return gen_set;
}

//compute similarity between 2 signatures (equal minhash values)
float compute_similarity(const Signature& sign1, const Signature& sign2)
{
    size_t count = 0;
    for (size_t hidx = 0; hidx < SIGNATURE_SIZE; hidx++)
    {
        if (sign1[hidx] == sign2[hidx])
            count++;
    }
    return float(count) / float(SIGNATURE_SIZE);
}

//compute both the jaccard similarity and the similarity between minhash signatures 
//for a given number of randomly generated sets
void get_similarity_coeffs(size_t set_size, size_t sets_no, const pair<size_t, size_t>& range,
    vector<vector<float>>& sim_coef_vec, vector<vector<float>>& jac_coef_vec)
{
    vector<set<int32_t>> sets;
    for (size_t idx = 0; idx < sets_no; idx++)
        sets.emplace_back(generate_set(set_size, range));

    //compute jaccard similarity
    std::vector<int> intersection;
    std::vector<int> sunion;
    jac_coef_vec.resize(sets_no, vector<float>(sets_no, 0));
    for (size_t i = 0; i < sets_no; i++)
    {
        for (size_t j = i + 1; j < sets_no; j++)
        {
            intersection.clear();
            sunion.clear();
            set_intersection(sets[i].begin(), sets[i].end(), sets[j].begin(), sets[j].end(), std::back_inserter(intersection));
            set_union(sets[i].begin(), sets[i].end(), sets[j].begin(), sets[j].end(), std::back_inserter(sunion));

            spdlog::info("intersection {}", intersection.size());
            jac_coef_vec[i][j] = float(intersection.size()) / float(sunion.size());
        }
    }

    //compute minhash signatures similarity
    MinHash minhash{SIGNATURE_SIZE, MINHASH_RANGE_MAX};
    sim_coef_vec.resize(sets_no, vector<float>(sets_no, 0));
    for (size_t i = 0; i < sets_no; i++)
        for (size_t j = i + 1; j < sets_no; j++)
            sim_coef_vec[i][j] = compute_similarity(minhash.compute_signature(sets[i]), minhash.compute_signature(sets[j]));
}

BOOST_AUTO_TEST_CASE(test_minhash_signatures)
{
    size_t sets_size = 300, sets_no = 50;
    vector<vector<float>> sim_coef_vec;
    vector<vector<float>> jac_coef_vec;
    get_similarity_coeffs(sets_size, sets_no, make_pair(1, 2000), sim_coef_vec, jac_coef_vec);
    vector<vector<float>> diff{sets_no, vector<float>(sets_no, 0)};
    int errors = 0;
    for (size_t i = 0; i < sets_no; i++)
    {
        for (size_t j = i + 1; j < sets_no; j++)
        {
            diff[i][j] = std::abs(sim_coef_vec[i][j] - jac_coef_vec[i][j]);
            spdlog::info("low collisions - similarity coefs: {} jaccard coefs: {}", sim_coef_vec[i][j], jac_coef_vec[i][j]);
            if (diff[i][j] > 0.1)
                errors++;
        }
    }
    BOOST_REQUIRE_LE(errors, 3);

    vector<vector<float>> sim_coef_vec_large;
    vector<vector<float>> jac_coef_vec_large;
    get_similarity_coeffs(sets_size, sets_no, make_pair(1, 500), sim_coef_vec_large, jac_coef_vec_large);
    errors = 0;
    for (size_t i = 0; i < sets_no; i++)
    {
        for (size_t j = i + 1; j < sets_no; j++)
        {
            spdlog::info("high collisions - similarity coefs: {} jaccard coefs: {}", sim_coef_vec_large[i][j], jac_coef_vec_large[i][j]);
            if (jac_coef_vec[i][j] > jac_coef_vec_large[i][j])
                errors++;
            if (sim_coef_vec[i][j] > sim_coef_vec_large[i][j])
                errors++;
        }
    }
    BOOST_REQUIRE_LE(errors, 3);
}
