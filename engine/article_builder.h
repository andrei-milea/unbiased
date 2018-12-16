#ifndef _ARTICLE_BUILDER_H
#define _ARTICLE_BUILDER_H

#include "utils/tokenize.h"
#include "utils/stemmer/porter2_stemmer.h"
#include "config.h"
#include "minhash.h"
#include "vocabulary.h"
#include "article.h"
#include "lsh_deduplication.h"

#include <functional>
#include <sstream>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>

enum class BuilderRes
{
	VALID,
	DUPLICATE,
	INVALID_WORDS
};

using boost::property_tree::ptree;

class ArticleBuilder
{
public:
	ArticleBuilder(const std::vector<std::pair<std::string, Signature>>& docs_signatures, float invalid_words_threshold = 0.3)
	:min_hash_(SIGNATURE_SIZE, MAX_WORDS_NO),
	lsh_deduplication_(SIGNATURE_SIZE, docs_signatures),
	invalid_words_threshold_(invalid_words_threshold)
	{
		std::ifstream ifs(Config::get().vocabulary_path);
        boost::archive::text_iarchive iarchive(ifs);
        iarchive >> vocabulary_;
	}

	~ArticleBuilder()
	{
		save_vocabulary();
	}

	void save_vocabulary()const
	{
		std::ofstream ofs(Config::get().vocabulary_path);
		boost::archive::text_oarchive oarchive(ofs);
		oarchive << vocabulary_;
	}

	const Vocabulary& get_vocabulary()const
	{
		return vocabulary_;
	}

	BuilderRes from_xml(const std::string &article_xml, Article &article)
    {
		ptree pt;
		std::stringstream ss;
		ss << article_xml;
		read_xml(ss, pt);
		article.url = pt.get<std::string>("article.url");
		article.title = pt.get<std::string>("article.title");
		article.date = pt.get<std::string>("article.date");
		auto authors_node = pt.get_child("article.authors");
		for(const auto& author_node : authors_node)
			article.authors.push_back(author_node.second.data());

		std::string text = pt.get<std::string>("article.text");
		article.length = text.size();
		auto tokens = tokenize(text);
		std::set<WordInt> words_ids;
		bool res = add_measures(tokens, article, words_ids);
		if(false == res)
			return BuilderRes::INVALID_WORDS;

		//increase words frequency for all words in the article
		for(WordInt word_id : words_ids)
			vocabulary_.increase_word_freq(word_id);
		article.duplicates = lsh_deduplication_.process_doc(std::make_pair(article.id, article.signature));
		if(article.duplicates.size() > 1)
			return BuilderRes::DUPLICATE;
		return BuilderRes::VALID;
	}

private:
	bool add_measures(const std::vector<std::string>& tokens, Article& article, std::set<WordInt> &word_ids)
	{
		std::set<uint32_t> shingles;
		article.tf.resize(vocabulary_.words_no(), 0.0);
		size_t words_no = 0, invalid_words_count = 0;
		for(size_t tidx = 0; tidx < tokens.size(); tidx++)
		{
			if(true == vocabulary_.is_stop_word(tokens[tidx])) //stop word detected
			{
				if(tidx + 2 < tokens.size())
				{
					auto shingle = get_shingle(tokens[tidx], tokens[tidx + 1], tokens[tidx + 2]);
					shingles.insert(shingle);
				}
			}
			else
			{
				words_no++;
				auto token = tokens[tidx];
				Porter2Stemmer::trim(token);
				Porter2Stemmer::stem(token);
				WordInt word_id = 0;
				if(true == vocabulary_.get_word_id(token, word_id))
				{
					assert(word_id < vocabulary_.words_no());
					article.tf[word_id]++;
					word_ids.insert(word_id);
				}
				else if(false/* == vocabulary_.is_misspelling(tokens[tidx])*/)
				{
					//deal with misspelings
				}
				else
				{
					//vocabulary_.add_new_word(token);
					invalid_words_count++;
				}
			}
		}
		article.words_no = words_no;
		article.unknown_words_no = invalid_words_count;
		if(float(invalid_words_count) / words_no > invalid_words_threshold_) 
			return false;

		//std::cout << "shingles: " << shingles.size() << std::endl;
		article.signature = min_hash_.compute_signature(shingles);
		return true;
	}

	//TODO fix 32-64bit combine
	uint32_t get_shingle(const std::string& w1, const std::string& w2, const std::string& w3)const
	{
		std::hash<std::string> hash_fct{};
        uint64_t hash_val = hash_fct(w1);
        uint32_t hash_val1 = hash_fct(w2);
        uint32_t hash_val2 = hash_fct(w3);
        boost::hash_combine(hash_val, hash_val1);
        boost::hash_combine(hash_val, hash_val2);
		return (uint32_t)hash_val;
	}

protected:
	Vocabulary vocabulary_;
	MinHash min_hash_;
	LSHDeduplication lsh_deduplication_;
	float invalid_words_threshold_;
};

#endif

