from nltk_utils import tfidf
from nltk_utils import tfidf_docs
from nltk_utils import corpus_vocab
from scrap import download_articles
import pymongo
import urllib
import numpy

class MongoUtils:
    def __init__(self, username, password, shard):
        self.client = pymongo.MongoClient("mongodb://" + username + ":" + urllib.parse.quote(password) + shard)
        self.db = self.client['unbiased']

    def load_vocabulary(self):
        collection = self.db['vocabulary']
        vocabulary_doc = collection.find_one({"english": "1"})
        if None == vocabulary_doc:
            raise Exception('no vocabulary found in database')
        return vocabulary_doc['words']

    def load_term_doc_matrix(self):
        collection = self.db['term_doc_matrix']
        term_doc_mat_entry = collection.find({})
        if None == term_doc_mat_entry:
            raise Exception('no term-doc matrix found in database')
        rows_no = term_doc_mat_entry[0]['rows']
        cols_no = term_doc_mat_entry[0]['cols']
        matrix_str = term_doc_mat_entry[0]['matrix'].strip('[]');
        term_doc_mat_array = numpy.fromstring(matrix_str, sep=' ')
        shape = (rows_no, cols_no)
        term_doc_mat = numpy.reshape(term_doc_mat_array, shape)
        return term_doc_mat

    def init_structures(self):
        articles_txt = download_articles('news_sites.txt')
        term_doc_mat, features = tfidf_docs(articles_txt)
        #self.init_vocabulary(features)
        self.init_term_doc_matrix(term_doc_mat)

    def init_vocabulary(self, new_words):
        collection = self.db['vocabulary']
        if collection.find({"english": "1"}).count() == 0:
            vocab_words = corpus_vocab()
            vocab_words.extend(new_words)
            vocab_words.sort()
            print('vocabulary size: ' + str(len(vocab_words)))
            vocabulary = {
                "english": "1",
                "words": vocab_words
            }
            collection.insert_one(vocabulary)

    def init_term_doc_matrix(self, term_doc_mat):
        collection = self.db['term_doc_matrix']
        if collection.find({}).count() == 0:
            mat_shape = term_doc_mat.shape
            print("term-doc matrix shape: " + str(mat_shape))
            numpy.set_printoptions(threshold=numpy.nan)
            matrix_doc = {
                "matrix": numpy.array2string(term_doc_mat.flatten()),
                "rows": mat_shape[0], 
                "cols": mat_shape[1],
            }
            collection.insert_one(matrix_doc)
    
def init_db():
    unbiased_shard = "@unbiased-shard-00-00-5jeo1.mongodb.net:27017,unbiased-shard-00-01-5jeo1.mongodb.net:27017,unbiased-shard-00-02-5jeo1.mongodb.net:27017/test?ssl=true&replicaSet=unbiased-shard-0&authSource=admin"
    mongo_utils = MongoUtils(username = "", password = "", shard = unbiased_shard)
    return mongo_utils

def main():
    try:
        unbiased_shard = "@unbiased-shard-00-00-5jeo1.mongodb.net:27017,unbiased-shard-00-01-5jeo1.mongodb.net:27017,unbiased-shard-00-02-5jeo1.mongodb.net:27017/test?ssl=true&replicaSet=unbiased-shard-0&authSource=admin"
        mongo_utils = MongoUtils(username = "rashomon", password = "ChangeIt!@34", shard = unbiased_shard)
        mongo_utils.init_structures()
    except Exception as exp:
        print('mongoutils caught error: ' + repr(exp))


if __name__ == "__main__":
    main()

