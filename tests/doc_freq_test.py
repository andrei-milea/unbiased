import pytest
from sklearn.feature_extraction.text import TfidfVectorizer
import numpy
from scipy.sparse import csr_matrix
import nltk
from nltk.stem.porter import PorterStemmer
from nltk.corpus import stopwords
from sklearn.decomposition import TruncatedSVD
import sys
import json
import dlib
import pudb
import term_doc_mat
from matrix_help import to_csr_mat

sys.path.insert(1, '../scripts')
from mongo_utils import MongoUtils
from article import from_xml

nltk.download('stopwords')

class Tokenizer(object):
    def __init__(self):
        nltk.download('punkt', quiet=True, raise_on_error=True)
        self.stemmer_ = nltk.stem.PorterStemmer()
        self.stop_words_ = stopwords.words('english')
        
    def _stem(self, token):
        #solves error "UserWarning: Your stop_words may be inconsistent with your preprocessing."
        if token in self.stop_words_:
            return token  
        return self.stemmer_.stem(token)
        
    def __call__(self, line):
        tokens = nltk.word_tokenize(line)
        tokens = (self._stem(token) for token in tokens)  # Stemming
        return list(tokens)

def test_tdidf():
    #load vocabulary
    mongo_utils = MongoUtils('unbiased')
    words = mongo_utils.load_vocabulary()
    idx = 0
    vocab = {}
    for word in words:
        vocab[word] = idx
        idx += 1

    assert 152588 == len(vocab), 'invalid vocabulary size'
    
    #articles_file = open(, 'r')
    articles = from_xml("articles.xml")
    porter_stemmer = PorterStemmer()
    tokenized_stop_words = nltk.word_tokenize(' '.join(nltk.corpus.stopwords.words('english')))
    tfidf_vect = TfidfVectorizer(norm=None, vocabulary=vocab, tokenizer=Tokenizer(), stop_words=tokenized_stop_words)
    #tfidf_vect = TfidfVectorizer(norm=None, tokenizer=Tokenizer(), stop_words=tokenized_stop_words)
    term_doc_mat_py = tfidf_vect.fit_transform(article.text for article in articles)
    print(term_doc_mat_py.shape)
    #term_doc_mat_cpp = dlib.matrix()
    term_doc_mat_cpp = term_doc_mat.compute_term_doc_mat()
    rows_no = term_doc_mat_cpp.nr()
    cols_no = term_doc_mat_cpp.nc()
    term_doc_mat_cpp_csr = to_csr_mat(term_doc_mat_cpp)

    assert term_doc_mat_cpp_csr == term_doc_mat_py.transpose()

    diff_mat = term_doc_mat_cpp_csr - term_doc_mat_py.transpose()
    for row in range(rows_no):
        for col in range(cols_no): 
            assert abs(diff_mat) < 0.5

    assert False, "end"


    #run LSA
    """
    svd_model = TruncatedSVD(n_components=20, algorithm='randomized', n_iter=1000, random_state=122)
    svd_model.fit(term_doc_mat)
    len(svd_model.components_)

    terms = tfidf_vect.get_feature_names()

    for idx, comp in enumerate(svd_model.components_):
        terms_comp = zip(terms, comp)
        sorted_terms = sorted(terms_comp, key= lambda x:x[1], reverse=True)[:7]
        print("Topic "+str(idx)+": ")
        for t in sorted_terms:
            print(t[0])
            print(" ")
    """

