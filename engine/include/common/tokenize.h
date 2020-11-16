#ifndef _TOKENIZE_H
#define _TOKENIZE_H

#include "porter2_stemmer.h"
#include <boost/locale.hpp>
#include <vector>
#include <string>

//using namespace boost::locale::boundary;

inline std::vector<std::string> tokenize(const std::string& text)
{
    boost::locale::generator gen;
    boost::locale::boundary::ssegment_index tokenizer(boost::locale::boundary::word, text.begin(), text.end(), gen("en_US.UTF-8")); 
    tokenizer.rule(boost::locale::boundary::word_any);
    std::vector<std::string> tokens(tokenizer.begin(), tokenizer.end());
    return tokens;
}

inline std::set<std::string> tokenize_stem(const std::string& text)
{
    boost::locale::generator gen;
    boost::locale::boundary::ssegment_index tokenizer(boost::locale::boundary::word, text.begin(), text.end(), gen("en_US.UTF-8")); 
    tokenizer.rule(boost::locale::boundary::word_any);
    std::set<std::string> tokens;
    for(auto tok_it = tokenizer.begin(); tok_it != tokenizer.end(); tok_it++)
    {
        std::string token_stem = *tok_it;
        Porter2Stemmer::trim(token_stem);
        Porter2Stemmer::stem(token_stem);
        tokens.insert(token_stem);
    }
    return tokens;
}

inline void trim_and_stem(std::string& token)
{
    Porter2Stemmer::trim(token);
    Porter2Stemmer::stem(token);
}

#endif

