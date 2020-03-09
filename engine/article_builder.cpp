#include "article_builder.h"
#include "utils/tokenize.h"
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/locale.hpp>
#include <vector>

using bsoncxx::builder::stream::finalize;
using bsoncxx::builder::stream::array_context;
using bsoncxx::builder::stream::open_array;
using bsoncxx::builder::stream::close_array;
using bsoncxx::builder::stream::open_document;
using bsoncxx::builder::stream::close_document;
using boost::property_tree::ptree;
using namespace std;

thread_local MongoDb ArticleBuilder::db_inst_;

BuilderRes ArticleBuilder::from_xml(const std::string &article_xml, Article &article)
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
		article.authors.emplace_back(author_node.second.data());

	std::string text = pt.get<std::string>("article.text");
	article.length = text.size();
	article.tokens = tokenize(text);
	vocabulary_.add_words_measures(article);
	if(float(article.unknown_words_no) / article.words_no > invalid_words_threshold_) 
		return BuilderRes::INVALID_WORDS;

	//std::cout << "shingles: " << shingles.size() << std::endl;
	article.signature = min_hash_.compute_signature(article.shingles);

	article.duplicates = lsh_deduplication_.process_doc(std::make_pair(article.id, article.signature));
	if(article.duplicates.size() > 1)
		return BuilderRes::DUPLICATE;
	
	//increase words frequency for all words in the article
        //and add token to stemid->tokens map
        //TODO - improve this
	for(const auto& idx_token : article.ids_tokens_map)
        {
		vocabulary_.increase_word_freq(idx_token.first);
		vocabulary_.add_token(idx_token.first, boost::locale::to_lower(article.tokens[idx_token.second]));
        }
	return BuilderRes::VALID;
}

void ArticleBuilder::save_vocabulary()const
{
	auto doc = to_bson(vocabulary_);
	ArticleBuilder::db_inst_.drop_collection("vocabulary");
	ArticleBuilder::db_inst_.save_doc("vocabulary", doc);
}

void ArticleBuilder::load_vocabulary()
{
	auto result = ArticleBuilder::db_inst_.get_docs("vocabulary");
	from_bson(*(result.begin()), vocabulary_);
}

void ArticleBuilder::update_vocab_freq()const
{
	auto doc = to_bson(vocabulary_, true);
	ArticleBuilder::db_inst_.update_doc("vocabulary", doc);
}

void ArticleBuilder::save_article(const Article& art)const
{
	auto doc = to_bson(art);
	ArticleBuilder::db_inst_.save_doc("articles", doc);
}

std::vector<Article> ArticleBuilder::load_articles()const
{
	auto result = ArticleBuilder::db_inst_.get_docs("articles");
	std::vector<Article> articles;
	for(const auto& doc : result)
	{
		Article article;
		from_bson(doc, article);
		articles.push_back(article);
	}
	return articles;
}

vector<pair<string, Signature>> ArticleBuilder::load_articles_signatures()const
{
	vector<pair<string, Signature>> articles_signatures;
	vector<string> fields({"_id", "signature"});
	auto result = ArticleBuilder::db_inst_.get_fields("articles", fields);
	for(const auto& doc : result)
	{
		pair<string, Signature> article_signature;
		from_bson(doc, article_signature);
		articles_signatures.push_back(article_signature);
	}
	return articles_signatures;
}

vector<pair<string, string>> ArticleBuilder::load_articles_dates(const vector<string> &articles_ids)const
{
	vector<pair<string, string>> articles_dates;
	bson_doc_bld filter_doc;
	filter_doc << "$or" << open_array << [&articles_ids](array_context<> arr)
							 	{
									for (const auto& article_id : articles_ids)
										arr << open_document << "id" << article_id << close_document;
							 	}	<< close_array;

	vector<string> fields({"_id", "date"});
	auto result = ArticleBuilder::db_inst_.get_fields("articles", fields, filter_doc);
	for(const auto& doc : result)
	{
		pair<string, string> art_date;
		from_bson(doc, art_date);
		articles_dates.push_back(art_date);
	}
	return articles_dates;
}

