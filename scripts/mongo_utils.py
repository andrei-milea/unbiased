from nltk import corpus
from nltk.corpus import stopwords
from nltk.stem import WordNetLemmatizer
import pymongo
import urllib
import numpy
import gc
import datetime
import xml.etree.ElementTree

class MongoUtils:
    def __init__(self, collection_name):
        server, username, password = self.__get_credentials("../bin/config.xml")
        print("mongodb://" + username + ":" + urllib.parse.quote(password) + "@" + server)
        self.client = pymongo.MongoClient("mongodb://" + username + ":" + urllib.parse.quote(password) + "@" + server)
        self.db = self.client[collection_name]

    def __get_credentials(self, config_filename):
        xml_root = xml.etree.ElementTree.parse(config_filename).getroot()
        mongo_elem = xml_root.find('mongodb')
        return mongo_elem[0].text, mongo_elem[1].text, mongo_elem[2].text

    def load_articles_meta(self):
        collection = self.db['articles']
        articles_meta = collection.find({})
        if None == articles_meta:
            raise Exception('no articles meta found in database')
        return articles_meta

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

    def clear_db(self):
        collection = self.db['term_doc_matrix']
        collection.drop()
        collection = self.db['articles_meta']
        collection.drop()
        collection = self.db['english_vocabulary']
        collection.drop()

def main():
    try:
        mongo_utils = MongoUtils()
        mongo_utils.save_corpus_vocabulary()
    except Exception as exp:
        print('mongoutils caught error: ' + repr(exp))

if __name__ == "__main__":
    main()

