#ifndef _ARTICLE_BUILDER_H
#define _ARTICLE_BUILDER_H

#include "minhash.h"
#include "vocabulary.h"
#include "article.h"

#include <functional>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/locale.hpp>

using boost::property_tree::ptree;
using namespace boost::locale::boundary;

const int HASH_FCTS_NO = 200;

class ArticleBuilder
{
public:
	ArticleBuilder(Vocabulary &vocabulary, uint64_t articles_no)
		:vocabulary_(vocabulary),
		articles_no_(articles_no),
		min_hash_(HASH_FCTS_NO, SIGNATURE_SIZE, vocabulary_.words_no())
	{}

	Article from_xml(const std::string &article_xml)const
    {
		Article article;
		ptree pt;
		read_xml(article_xml, pt);
		article.meta_.url = pt.get<std::string>("article.url");
		article.meta_.title = pt.get<std::string>("article.title");
		article.meta_.date = pt.get<std::string>("article.date");
		auto authors_node = pt.get_child("article.authors");
		for(const auto& author_node : authors_node)
			article.meta_.authors.push_back(author_node.second.get<std::string>("author"));

		std::string text = pt.get<std::string>("article.text");
		add_similarity_measures(text, article.similarity_.tf, article.similarity_.idf, article.similarity_.signature);
		return article;
	}

protected:

	void add_similarity_measures(const std::string& text, std::vector<double> &tf_vec, std::vector<double>& idf_vec, Signature& signature)const
	{
		std::set<uint32_t> shingles;
		boost::locale::generator gen;
		boost::locale::boundary::ssegment_index tokenizer(boost::locale::boundary::word, text.begin(), text.end(), gen("en_US.UTF-8")); 
		auto tok_end = tokenizer.end();
		std::unordered_map<std::string, WordInfo> tf_map;
		for(auto word_it = tokenizer.begin(); word_it != tok_end; ++word_it)
		{
			if(true == vocabulary_.is_stop_word(*word_it)) //stop word detected
			{
				auto next_word_it = word_it;
				auto shingle = get_shingle(*word_it, *(++next_word_it), *(++next_word_it));
				shingles.insert(shingle);
			}
			else
			{
				WordInt word_id = 0;
				if(true == vocabulary_.get_word_id(*word_it, word_id))
				{
					tf_map[*word_it].word_id = word_id;
					tf_map[*word_it].freq++;
					vocabulary_.increase_word_freq(*word_it);
				}
				else if(true/* == vocabulary_.is_misspelling(*word_it)*/)
				{
					//deal with misspelings
				}
				else
					vocabulary_.add_new_word(*word_it);
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
	Vocabulary &vocabulary_;
	uint64_t articles_no_;
	MinHash min_hash_;
};

#endif

