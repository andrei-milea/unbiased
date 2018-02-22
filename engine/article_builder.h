#ifndef _ARTICLE_BUILDER_H
#define _ARTICLE_BUILDER_H

#include "utils/tokenize.h"
#include "utils/stemmer/porter2_stemmer.h"
#include "minhash.h"
#include "vocabulary.h"
#include "article.h"
#include "lsh_deduplication.h"

#include <functional>
#include <sstream>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>

using boost::property_tree::ptree;

class ArticleBuilder
{
public:
	ArticleBuilder(uint64_t articles_no, const std::vector<std::pair<std::string, Signature>>& docs_signatures)
	:articles_no_(articles_no),
	min_hash_(SIGNATURE_SIZE, MAX_WORDS_NO),
	lsh_deduplication_(SIGNATURE_SIZE, docs_signatures)
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

	Article from_xml(const std::string &article_xml)
    {
		Article article;
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
		add_similarity_measures(tokens, article.tf, article.idf, article.signature);
		article.source = lsh_deduplication_.process_doc(std::make_pair(article.id, article.signature));
		articles_no_++;
		return article;
	}

	void add_similarity_measures(const std::vector<std::string>& tokens, std::vector<double> &tf_vec, std::vector<double>& idf_vec, Signature& signature)
	{
		std::set<uint32_t> shingles;
		
		std::unordered_map<std::string, WordInfo> tf_map;
		for(size_t tidx = 0; tidx < tokens.size(); tidx++)
		{
			if(true == vocabulary_.is_stop_word(tokens[tidx])) //stop word detected
			{
				if(tidx + 2 > tokens.size())
				{
					auto shingle = get_shingle(tokens[tidx], tokens[tidx + 1], tokens[tidx + 2]);
					shingles.insert(shingle);
				}
			}
			else
			{
				auto token = tokens[tidx];
				Porter2Stemmer::trim(token);
				Porter2Stemmer::stem(token);
				WordInt word_id = 0;
				if(true == vocabulary_.get_word_id(token, word_id))
				{
					tf_map[token].word_id = word_id;
					tf_map[token].freq++;
					vocabulary_.increase_word_freq(token);
				}
				else if(true/* == vocabulary_.is_misspelling(tokens[tidx])*/)
				{
					//deal with misspelings
				}
				else
					vocabulary_.add_new_word(token);
			}
		}
		compute_tfidf(tf_map, tf_vec, idf_vec);
		signature = min_hash_.compute_signature(shingles);
	}

	void compute_tfidf(const std::unordered_map<std::string, WordInfo>& tf_map, std::vector<double> &tf, std::vector<double> &idf)const
	{
		tf.resize(vocabulary_.words_no());
		idf.resize(vocabulary_.words_no());
		for(const auto& el : tf_map)
		{
			tf[el.second.word_id] = (double(el.second.freq) + 1.0);
	   		idf[el.second.word_id] = std::log(double(articles_no_) / vocabulary_.get_word_freq(el.first)) + 1.0;
		}
	}

	uint32_t get_shingle(const std::string& w1, const std::string& w2, const std::string& w3)const
	{
		std::hash<std::string> hash_fct{};
        uint32_t hash_val = hash_fct(w1);
        boost::hash_combine(hash_val, hash_fct(w2));
        boost::hash_combine(hash_val, hash_fct(w3));
		return hash_val;
	}

protected:
	Vocabulary vocabulary_;
	uint64_t articles_no_;
	MinHash min_hash_;
	LSHDeduplication lsh_deduplication_;
};

#endif

