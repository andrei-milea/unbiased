#include "lsa.h"

using namespace dlib;
using namespace std;

void LSA::build_term_doc_matrix(const std::vector<Article> &articles, size_t all_articles_no)
{
	assert(articles.size() > 0 && all_articles_no > 0);
	articles_no_ = double(all_articles_no);
	build_vocab_to_mat_idx(articles);
	init_term_doc_matrix(terms_mat_to_vocab_.size(), articles.size());
	for(size_t cidx = 0; cidx < articles.size(); cidx++)
	{
		for(size_t ridx = 0; ridx < terms_mat_to_vocab_.size(); ridx++)
		{
			size_t vocab_idx = terms_mat_to_vocab_.at(ridx);
			if(articles[cidx].tf[vocab_idx] != 0.0)
				term_doc_mat_(ridx, cidx) = articles[cidx].tf[vocab_idx] / articles[cidx].words_no
											* log(articles_no_ / vocabulary_.get_word_freq(vocab_idx));
		}
	} 
}

void LSA::print_term_doc_matrix()const
{
	cout << "term_doc_mat size: " << term_doc_mat_.nr() << " " << term_doc_mat_.nc() << "\n";
	for(long row = 0; row < term_doc_mat_.nr(); row++)
	{
		for(long col = 0; col < term_doc_mat_.nc(); col++)
			cout << term_doc_mat_(row, col) << " ";
		cout << "\n";
	}
}

void LSA::print_sigma()const
{
	cout << "sigma size: " << sigma_.nr() << " " << sigma_.nc() << "\n";
	for(long row = 0; row < sigma_.nr(); row++)
	{
		for(long col = 0; col < sigma_.nc(); col++)
			cout << sigma_(row, col) << " ";
		cout << "\n";
	}
}

void LSA::run_svd(const std::vector<Article> &articles, size_t all_articles_no)
{
	build_term_doc_matrix(articles, all_articles_no);
	svd(term_doc_mat_, U_, sigma_, V_);
}

std::vector<std::set<size_t>> LSA::get_docs_keywords(float doc_threshold, float term_threshold, size_t concepts_no)const
{
	std::vector<std::set<size_t>> docs_keywords(V_.nr());
	for(size_t doc_idx = 0; doc_idx < V_.nr(); doc_idx++)
	{
		for(size_t concept_idx = 0; concept_idx < concepts_no; concept_idx++)
		{
			if(V_(doc_idx, concept_idx) > doc_threshold)
			{
				for(size_t term_idx = 0; term_idx < U_.nr(); term_idx++)
					if(U_(term_idx, concept_idx) > term_threshold)
						docs_keywords[doc_idx].insert(terms_mat_to_vocab_[term_idx]);
			}
		}
	}
	return docs_keywords;
}

dlib::matrix<double> LSA::foldin_doc(const Article& art, size_t concepts_no)const
{
	//TODO
	dlib::matrix<double> doc_vec;
	for(size_t ridx = 0; ridx < terms_mat_to_vocab_.size(); ridx++)
	{
		size_t vocab_idx = terms_mat_to_vocab_.at(ridx);
		if(art.tf[vocab_idx] != 0.0)
			doc_vec(ridx) = art.tf[vocab_idx] / art.words_no
										* log(articles_no_ / vocabulary_.get_word_freq(vocab_idx));
		else
			doc_vec(ridx) = 0.0;
	}
	return doc_vec;
}

void LSA::print_top_concepts(const std::vector<Article> &articles, double threshold)const
{
	std::unordered_map<size_t, std::vector<size_t>> concept_doc_map;
	std::unordered_map<size_t, std::vector<size_t>> concept_term_map;
	cout << "U_ size: " << U_.nr() << " " << U_.nc() << "\n";
	//rows are terms, cols are concepts
	for(long col = 0; col < 50/*U_.nc()*/; col++)
	{
		cout << "CONCEPT START\n";
		for(long row = 0; row < U_.nr(); row++)
		{
			if(U_(row, col) > threshold)
			{
				string word;
				bool res = vocabulary_.get_word(terms_mat_to_vocab_[row], word);
				concept_term_map[col].push_back(terms_mat_to_vocab_[row]);
				assert(res);
				cout << word << " ";
			}
			if(row < V_.nr() && V_(row, col) > threshold)
				concept_doc_map[col].push_back(row);
		}
		cout << "CONCEPT END\n";
	}
	for(const auto& concept_doc : concept_doc_map)
	{
		cout << "CONCEPT--" << endl;
		for(const auto art_idx : concept_doc.second)
		{
			cout << "article: " << articles[art_idx].title << "\n";
			for(const auto& word_idx : concept_term_map[concept_doc.first])
			{
				auto it = articles[art_idx].ids_tokens_map.find(word_idx);
				if(it != articles[art_idx].ids_tokens_map.end())
				{
					cout << articles[art_idx].tokens[it->second] << endl;
				}
			}
		}
		cout << endl;
	}
}


