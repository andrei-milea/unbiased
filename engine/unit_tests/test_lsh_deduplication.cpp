#define BOOST_TEST_MODULE "test_lsh_deduplication"

#include "config.h"
#include "log_helper.h"
#include "lsh_deduplication.h"
#include "boost/test/included/unit_test.hpp"
#include <random>
#include <vector>
#include <string>

using namespace boost::unit_test;
using namespace std;

RegisterUnitTestLogger register_logger{BOOST_TEST_MODULE};

char alphabet[26] = { 'a', 'b', 'c', 'd', 'e', 'f', 'g', 
                          'h', 'i', 'j', 'k', 'l', 'm', 'n',  
                          'o', 'p', 'q', 'r', 's', 't', 'u', 
                          'v', 'w', 'x', 'y', 'z' };

void generate_signatures(int32_t signatures_no, std::vector<std::pair<std::string, Signature>> &signatures)
{
    std::random_device rd;
    std::mt19937 gen{rd()};
    std::uniform_int_distribution<> dis{0, 25};

    for (int32_t sign_idx = 0; sign_idx < signatures_no; sign_idx++)
    {
        Signature signature;
        string str;
        for (int32_t pos = 0; pos < signature.size(); pos++)
        {
            str.push_back(alphabet[dis(gen)]);
            signature[pos] = dis(gen);
        }
        str += to_string(sign_idx);
        signatures.push_back(make_pair(str, signature));
    }
}

void alter_signature(Signature & sign, int32_t rand_diffs = 10)
{
    std::random_device rd;
    std::mt19937 gen{rd()};
    std::uniform_int_distribution<> pos_dis{0, sign.size() - 1};
    std::uniform_int_distribution<> val_dis{0, 25};
    for (int32_t diff_idx = 0; diff_idx < rand_diffs; diff_idx++)
        sign[pos_dis(gen)] = val_dis(gen);
}

BOOST_AUTO_TEST_CASE(test_lsh_deduplication_identical)
{
    std::vector<std::pair<std::string, Signature>> signatures;
    generate_signatures(1000, signatures);
    LSHDeduplication lsh_deduplication{SIGNATURE_SIZE, signatures};
    int32_t count = 0;
    for (auto& signature : signatures)
    {
        signature.first += string("dup");
        auto duplicates = lsh_deduplication.process_signature(signature);
        spdlog::info("duplicates count {}", duplicates.size());
        if (duplicates.size() > 2)
        {
            count++;
            spdlog::info("DUPLICATES START");
            for(const auto& signature_str : duplicates)
                spdlog::info("duplicate signature {}", signature_str);
            spdlog::info("DUPLICATES END");
        }
        //duplicated signatures should hash to existing signatures
        BOOST_CHECK_GE(duplicates.size(), 2);
    }
    //less than 0.1 invalid duplicates 
    BOOST_CHECK_LE(count, 10);
}


BOOST_AUTO_TEST_CASE(test_lsh_deduplication_similar)
{
    std::vector<std::pair<std::string, Signature>> signatures;
    generate_signatures(1000, signatures);
    LSHDeduplication lsh_deduplication{SIGNATURE_SIZE, signatures};
    int32_t count_duplicates = 0, count_non_duplicates = 0;
    for (auto& signature : signatures)
    {
        signature.first += string("dup");
        alter_signature(signature.second);
        auto duplicates = lsh_deduplication.process_signature(signature);
        spdlog::info("duplicates count {}", duplicates.size());
        if (duplicates.size() > 2)
            count_duplicates++;
        else if (duplicates.size() == 1)
            count_non_duplicates++;
    }
    //less than 0.1 invalid duplicates 
    BOOST_CHECK_LE(count_duplicates, 5);
    BOOST_CHECK_LE(count_non_duplicates, 5);
}
