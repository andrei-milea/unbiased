import os
from article import Article
from bs4 import BeautifulSoup
import logging
from nltk.corpus import stopwords
from sklearn.feature_extraction.text import CountVectorizer
import collections, re

class ReutersData:
    """manipulation and extraction for reuters data"""
    articles_ = []
    train_articles_text_ = []
    test_articles_text_ = []

    def __init__(self, path):
        if os.path.isdir(path):
            for filename in os.listdir(path): 
                if filename.find("xml") == -1:
                    continue
                with open(path+ "/" + filename, 'r', encoding='iso-8859-15') as in_file:
                    content = in_file.read()
                    soup = BeautifulSoup(content, 'lxml-xml')
                    articles_xml = soup.findAll('REUTERS')
                    for article_xml in articles_xml:
                        author = "no_author"
                        title = "no_title"
                        date = "no_date"
                        places = "no_places"
                        people = "no_people"
                        orgs = "no_orgs"
                        text = "no_text"
                        topics = "no_topics"
                        
                        if article_xml.AUTHOR != None:
                            author = article_xml.AUTHOR.string
                        if article_xml.TITLE != None:
                            title = article_xml.TITLE.string
                        if article_xml.DATE != None:
                            date = article_xml.DATE.string
                        if article_xml.TEXT != None:
                            text = article_xml.TEXT.text
                        if article_xml.PLACES != None:
                            places = [child.string for child in article_xml.PLACES.children]
                        if article_xml.PEOPLE != None:
                            people = [child.string for child in article_xml.PEOPLE.children]
                        if article_xml.ORGS != None:
                            orgs = [child.string for child in article_xml.ORGS.children]
                        if article_xml.TOPICS != None:
                            topics = [child.string for child in article_xml.TOPICS.children]
                        article = Article(author, title, date, text, places, people, orgs, topics)
                        self.articles_.append(article)
                        if topics:
                            self.train_articles_text_.append(text)
                            print(''.join(topics) + "\n")
                        else:
                            self.test_articles_text_.append(text)
        #logging.warning(len(self.articles_))

    def train(self):
        eng_stop_words = set(stopwords.words("english"))
        vectorizer = CountVectorizer(analyzer = "word", tokenizer = None, preprocessor = None, stop_words = eng_stop_words, max_features = 5000)
        train_data_features = vectorizer.fit_transform(self.train_articles_text_)
        #logging.warning(vectorizer.get_feature_names())
        train_data_features = train_data_features.toarray()
        print(train_data_features.shape)

            

def main():
    reuters_data = ReutersData("../data/reuters")
    reuters_data.train()

if __name__ == "__main__":
    main()
 
