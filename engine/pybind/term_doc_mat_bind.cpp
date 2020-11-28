#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <boost/locale.hpp>

#include "config.h"
#include "lsa.h"
#include "vocabulary.h"
#include "article_parser.h"
#include "article_utils.h"
#include "perf_clock.h"

const string words_filename{"words.dat"};
const string stopwords_filename{"stop_words.dat"};

namespace py = pybind11;
using namespace std;

dlib::matrix<double> compute_term_doc_mat()
{
    Vocabulary vocab { words_filename, stopwords_filename };
    ArticleParser article_parser{ vocab };
    auto articles_xml = load_articles_xml("articles.xml");
    std::unordered_set<string> titles;
    std::vector<Article> articles;
    for (const auto& article_xml : articles_xml)
    {
        Article new_article;
        article_parser.parse_from_xml(article_xml, new_article);
        if (titles.count(new_article.meta_data.title) == 0)
        {
            titles.insert(new_article.meta_data.title);
            set<string> stems;
            bool valid = article_parser.tokenize_validate(new_article, stems);
            if (valid)
            {
                vocab.add_stems(std::move(stems));
                articles.push_back(std::move(new_article));
            }
        }
    }
    std::vector<ProcessedArticle> processed_articles;
    for (auto&& article : articles)
    {
        ProcessedArticle proc_article;
        article_parser.process_tokens(article, proc_article);
        processed_articles.push_back(std::move(proc_article));
    }

    LSA lsa_processor { vocab };
    lsa_processor.build_term_doc_matrix(processed_articles, 3);
    return lsa_processor.get_term_doc_matrix();
}

pair<dlib::matrix<double>, dlib::matrix<double>> compute_lsa_mat()
{
    pair<dlib::matrix<double>, dlib::matrix<double>> res;
    try
    {
        Vocabulary vocab { words_filename, stopwords_filename };
        ArticleParser article_parser{ vocab };
        auto articles_xml = load_articles_xml("articles.xml");
        std::unordered_set<string> titles;
        std::vector<Article> articles;
        for (const auto& article_xml : articles_xml)
        {
            Article new_article;
            article_parser.parse_from_xml(article_xml, new_article);
            if (titles.count(new_article.meta_data.title) == 0)
            {
                titles.insert(new_article.meta_data.title);
                set<string> stems;
                bool valid = article_parser.tokenize_validate(new_article, stems);
                if (valid)
                {
                    vocab.add_stems(std::move(stems));
                    articles.push_back(std::move(new_article));
                }
            }
        }
        std::vector<ProcessedArticle> processed_articles;
        for (auto&& article : articles)
        {
            ProcessedArticle proc_article;
            article_parser.process_tokens(article, proc_article);
            processed_articles.push_back(std::move(proc_article));
        }

        LSA lsa_processor { vocab };
        lsa_processor.run_svd(processed_articles);
        res.first = lsa_processor.get_terms_concepts_mat();
        res.second = lsa_processor.get_docs_concepts_mat();
    }
    catch (const std::exception& ex)
    {
        cout << ex.what();
    }
    return res;
}

std::vector<set<string>> compute_topics(int64_t concepts_no, int64_t terms_no)
{
    LogRunTime log_runtime("compute_topics");
    std::vector<set<string>> res;
    try
    {
        Vocabulary vocab { words_filename, stopwords_filename };
        ArticleParser article_parser{ vocab };
        auto articles_xml = load_articles_xml("articles.xml");
        std::unordered_set<string> titles;
        std::vector<Article> articles;
        for (const auto& article_xml : articles_xml)
        {
            Article new_article;
            article_parser.parse_from_xml(article_xml, new_article);
            if (titles.count(new_article.meta_data.title) == 0)
            {
                titles.insert(new_article.meta_data.title);
                set<string> stems;
                bool valid = article_parser.tokenize_validate(new_article, stems);
                if (valid)
                {
                    vocab.add_stems(std::move(stems));
                    articles.push_back(std::move(new_article));
                }
            }
        }
        log_runtime.log("parsing articles and loading vocab");
        std::vector<ProcessedArticle> processed_articles;
        for (auto&& article : articles)
        {
            ProcessedArticle proc_article;
            article_parser.process_tokens(article, proc_article);
            processed_articles.push_back(std::move(proc_article));
        }
        log_runtime.log("processing articles");

        LSA lsa_processor { vocab };
        lsa_processor.run_svd(processed_articles);
        //lsa_processor.print_term_doc_matrix();
        //lsa_processor.print_sigma();
        log_runtime.log("runnig svd");

        res = lsa_processor.get_top_concepts(processed_articles, concepts_no, terms_no);
        log_runtime.log("get_top_concepts");
    }
    catch (const std::exception& ex)
    {
        cout << ex.what();
    }
    return res;
}

double get_mat_elem(dlib::matrix<double>& mat, int row, int col)
{
    return mat(row, col);
}

PYBIND11_MODULE(term_doc_mat, m)
{
    m.doc() = "pybind11 compute term_doc_mat"; // optional module docstring
    py::class_<dlib::matrix<double>>(m, "matrix",
        "This object represents a dense 2D matrix of floating point numbers."
        "Moreover, it binds directly to the C++ type dlib::matrix<double>.")
        .def(py::init<>())
        .def("nr", &dlib::matrix<double>::nr, "Return the number of rows in the matrix.")
        .def("nc", &dlib::matrix<double>::nc, "Return the number of columns in the matrix.");

    m.def("compute_term_doc_mat", &compute_term_doc_mat, "compute term-document matrix");
    m.def("compute_lsa_mat", &compute_lsa_mat, "compute lsa term-concept and doc_concept matrices");
    m.def("compute_topics", &compute_topics, "compute all topics with their associated terms");
    m.def("get_mat_elem", &get_mat_elem, "return matrix element");
}
