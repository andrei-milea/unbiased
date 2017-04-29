import os
from article import Article
from bs4 import BeautifulSoup
import logging

class ReutersData:
    """manipulation and extraction for reuters data"""
    _articles = []

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
                        self._articles.append(article)
        #logging.warning(len(self._articles))

    #def train(self):
        #for article in self._articles:

            

def main():
    reuters_data = ReutersData("../data/reuters")
    #reuters_data.train()

if __name__ == "__main__":
    main()
 
