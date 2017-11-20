from nltk_utils import tfidf
from nltk_utils import tfidf_docs
from nltk_utils import corpus_vocab
import pymongo
import urllib

class MongoUtils:
    def __init__(self, username, password, shard):
        self.client = pymongo.MongoClient("mongodb://" + username + ":" + urllib.parse.quote(password) + shard)
        self.db = self.client['unbiased']

    def load_vocabulary(self)
        vocabulary_doc = collection.find_one({"english": "1"})
        if None == vocabulary_doc:
            raise Exception('no vocabulary found in database')
        return vocabulary_doc.words

    def init_structures(self):
        articles = download_articles('news_sites.txt')
        term_doc_mat, features = tfidf_docs(articles_txt)
        #init vocabulary
        collection = self.db['vocabulary']
        if collection.find({"english": "1"}).count() == 0:
            vocab_words = corpus_vocab()
            vocab_words.extend(features)
            vocab_words.sort()
            print('vocabulary size: ' + words)
            vocabulary = {
                "english": "1",
                "words": vocab_words
            }
            collection.insert_one(vocabulary)
        #init term-doc matrix
        collection = self.db['term_doc_matrix']
        if collection.find({}).count() > 0:
            mat_shape = term_doc_mat.shape
            print("term-doc matrix shape: " + str(mat_shape))
            matrix_doc = {
                "matrix": term_doc_mat.toarray().flatten(),
                "rows": mat_shape[0], 
                "cols": mat_shape[1],
            }
            collection.insert_one(matrix_doc)
    

def main():
    unbiased_shard = "@unbiased-shard-00-00-5jeo1.mongodb.net:27017,unbiased-shard-00-01-5jeo1.mongodb.net:27017,unbiased-shard-00-02-5jeo1.mongodb.net:27017/test?ssl=true&replicaSet=unbiased-shard-0&authSource=admin"
    mongo_utils = MongoUtils(username = "rashomon", password = "ChangeIt!@34", shard = unbiased_shard)
    mongo_utils.store_structures()

if __name__ == "__main__":
    main()

