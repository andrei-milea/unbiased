import collections, re
import nltk
#nltk.download()
from nltk.corpus import stopwords
from sklearn.feature_extraction.text import CountVectorizer
import logging

class Article:
    _author = ""
    _title = ""
    _date = ""
    _text = ""
    _bow = []
    _places = []
    _people = []
    _orgs = []
    _topics = []

    def __init__(self, author, title, date, text, places, people, orgs, topics):
        self._author = author
        self._title = title
        self._date = date
        self._text = text
        self._places = places
        self._people = people
        self._orgs = orgs
        text_filtered = re.sub("[^a-zA-Z]", " ", self._text)
        text_filtered = text_filtered.lower()
        words = text_filtered.split()
        stops = set(stopwords.words("english"))
        words = [w for w in words if not w in stops]
        _topics = topics
        #logging.warning(" ".join(words))
        #self._bow = collections.Counter(re.findall(r'\w+', " ".join(words)))



