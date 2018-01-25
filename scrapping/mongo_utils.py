from nltk_utils import tfidf
from nltk_utils import tfidf_docs
from nltk_utils import corpus_vocab
from scrap import download_article
from scrap import scrap_vocabulary
import pymongo
import urllib
import numpy
import gc
import datetime
import xml.etree.ElementTree

def get_credentials(config_filename):
    xml_root = xml.etree.ElementTree.parse(config_filename).getroot()
    mongo_elem = xml_root.find('mongodb')
    return mongo_elem[0], mongo_elem[1], mongo_elem[2]

class MongoUtils:
    def __init__(self, username, password, server):
        self.client = pymongo.MongoClient("mongodb://" + username + ":" + urllib.parse.quote(password) + "@" + server)
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

    def load_articles_meta(self):
        collection = self.db['articles_meta']
        articles_meta = collection.find({})
        if None == articles_meta:
            raise Exception('no articles meta found in database')
        return articles_meta[0]

    def save_articles_meta(self, vocabulary):
        articles_urls = get_articles_urls('news_sites.txt')
        timestamp = str(datetime.datetime.now())
        for article_url in articles_urls:
            article = download_article(article_url)
            tfidf_vec = tfidf(article.title + ' ' + article.text, vocabulary)
            self.save_article_meta(article.url, article.title, article.authors, tfidf_vec, timestamp)
            del article
            gc.collect()

    def save_corpus_vocabulary(self):
        collection = self.db['vocabulary']
        vocab_words, stop_words = corpus_vocab()
        words_data = []
        count = 0
        for vocab_word in vocab_words:
            words_data.append({"word" : vocab_word, "id" : count, "frequency" : 0})
            count += 1
        vocabulary = {
            "language": "english",
            "words" : words_data,
            "stop_words": vocab_words
        }
        collection.insert_one(vocabulary)

    def save_article_meta(self, url, title, authors, tfidf_vec, timestamp):
        collection = self.db['articles_meta']
        articles_doc = {
            "timestamp": timestamp,
            "url": url,
            "title": title,
            "authors": authors,
            "tfidf": tfidf_vec
        }
        collection.insert_one(articles_doc)

    def clear_db(self):
        collection = self.db['term_doc_matrix']
        collection.drop()
        collection = self.db['articles_meta']
        collection.drop()
        collection = self.db['vocabulary']
        collection.drop()

    def save_term_doc_matrix(self, term_doc_mat, timestamp):
        collection = self.db['term_doc_matrix']
        if collection.find({}).count() == 0:
            mat_shape = term_doc_mat.shape
            print("term-doc matrix shape: " + str(mat_shape))
            numpy.set_printoptions(threshold=numpy.nan)
            matrix_doc = {
                "timestamp": timestamp,
                "matrix": numpy.array2string(term_doc_mat.flatten()),
                "rows": mat_shape[0], 
                "cols": mat_shape[1],
            }
            collection.insert_one(matrix_doc)
    
def init_db():
    unbiased_shard = "@unbiased-shard-00-00-5jeo1.mongodb.net:27017,unbiased-shard-00-01-5jeo1.mongodb.net:27017,unbiased-shard-00-02-5jeo1.mongodb.net:27017/test?ssl=true&replicaSet=unbiased-shard-0&authSource=admin"
    serv, user, passw = get_credentials("../bin/config.xml")
    mongo_utils = MongoUtils(username = user, password = passw, server = serv)
    return mongo_utils

def main():
    try:
        mongo_utils = init_db()
        mongo_utils.save_vocabulary('news_sites.txt')
    except Exception as exp:
        print('mongoutils caught error: ' + repr(exp))


if __name__ == "__main__":
    main()

