#ifndef _ARTICLE_BUILDER_H
#define _ARTICLE_BUILDER_H

#include "types.h"
#include "minhash.h"
#include "vocabulary.h"
#include "article.h"

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/tokenizer.hpp>

using boost::property_tree::ptree;

class ArticleBuilder
{
public:
	ArticleBuilder(const Vocabulary &vocabulary)
		:vocabulary_(vocabulary)
	{}

	Article build_article(const std::string &article_str)
    {
		ptree pt;
		read_xml(article_str, pt);
		title_ = pt.get<std::string>("article.title");
		date_ = pt.get<std::string>("article.date");
		auto authors_node = pt.get_child("article.authors");
		for(const auto& author_node : authors_node)
			authors_.push_back(author_node.second.get<std::string>("author"));
		auto keywords_node = pt.get_child("article.keywords");
		for(const auto& keyword_node : keywords_node)
			keywords_.push_back(keyword_node.second.get<std::string>("keyword"));

		auto words_str = pt.get<std::string>("article.words");
		std::vector<WordInt> words;
		compute_words(words_str, words);
		compute_signature(words);
	}

	void compute_signature(const std::vector<WordInt> &words)
	{
		std::set<TriGram> trigrams;
		compute_ngrams(words, trigrams);
		MinHash<TriGram> minhash(200, vocabulary_.size());
		minhash.compute_signature(trigrams, signature_);
	}

	void compute_ngrams(const std::vector<WordInt> &words, std::set<TriGram>& trigrams)const
	{
		size_t idx = 0;
		while(idx < words.size())
		{
			if(idx + 3 < words.size() && words[idx] != 0 && words[idx + 1] != 0 && words[idx + 2] != 0)
				trigrams.insert(TriGram{{words[idx], words[idx + 1], words[idx + 2]}});
			else if(words[idx + 1] != 0)
				idx += 2;
			else if(words[idx] != 0)
				idx += 1;
			idx += 3;
		}
	}

	void compute_words(const std::string& words_str, std::vector<WordInt> &words)
	{
		boost::char_separator<char> sep(",;. ");
		boost::tokenizer< boost::char_separator<char> > tok(words_str, sep);
		for(auto tok_it = tok.begin(); tok_it != tok.end(); tok_it++)
		{
			WordInt word_id = 0;
			bool res = vocabulary_.get_word_id(*tok_it, word_id);
			if(false == res)
			{
				vocabulary_.add_word(*tok_it);
				words.push_back(UNKNOWN_WORD);
			}
			else
				words.push_back(word_id);
		}
	}

protected:
	const Vocabulary &vocabulary_;
};

#endif

