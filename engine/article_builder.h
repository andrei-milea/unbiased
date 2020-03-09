#ifndef _ARTICLE_BUILDER_H
#define _ARTICLE_BUILDER_H

#include "config.h"
#include "minhash.h"
#include "bson_builder.h"
#include "mongodb.h"
#include "vocabulary.h"
#include "article.h"
#include "lsh_deduplication.h"
#include <functional>
#include <boost/locale.hpp>

const size_t MINHASH_RANGE_MAX = 100000000;

enum class BuilderRes
{
	VALID,
	DUPLICATE,
	INVALID_WORDS
};

class ArticleBuilder : public BsonBuilder
{
public:
	explicit ArticleBuilder(float invalid_words_threshold = 0.3)
		:min_hash_(SIGNATURE_SIZE, MINHASH_RANGE_MAX),
		lsh_deduplication_(SIGNATURE_SIZE, load_articles_signatures()),
		invalid_words_threshold_(invalid_words_threshold)
	{
            //create system default locale
            boost::locale::generator gen;
            std::locale loc=gen(""); 
            //make it system global
            std::locale::global(loc); 

            load_vocabulary();
	}

	~ArticleBuilder()
	{
		//save_vocabulary();
	}

	void save_vocabulary()const;

	void load_vocabulary();

	void update_vocab_freq()const;

	const Vocabulary& get_vocabulary()const noexcept
	{
		return vocabulary_;
	}

	BuilderRes from_xml(const std::string &article_xml, Article &article);

	void save_article(const Article& art)const;

	std::vector<Article> load_articles()const;

public:
	static thread_local MongoDb db_inst_;

protected:
	std::vector<std::pair<std::string, std::string>> load_articles_dates(const std::vector<std::string> &articles_ids)const;

	std::vector<std::pair<std::string, Signature>> load_articles_signatures()const;

protected:
	Vocabulary vocabulary_;
	MinHash min_hash_;
	LSHDeduplication lsh_deduplication_;
	float invalid_words_threshold_;
};

#endif

