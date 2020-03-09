import logging
from newspaper import Article
from lxml import etree

def to_xml(article):
    result = "<article>"
    result += "<url>" + article.url + "</url>"
    if article.publish_date:
        date = article.publish_date.strftime("%Y-%m-%d %H:%M:%S")
    else:
        date = ""
    result += "<date>" + date + "</date>"
    result += "<title>" + article.title + "</title>"
    result += "<authors>"
    for author in article.authors:
        result += "<author>" + author + "</author>"
    result += "</authors>"
    result += "<text>" + article.text + "</text>"
    result += "</article>"
    return result

def from_xml(articles_xml_file):
    xml_file = open(articles_xml_file, "r")
    contents = xml_file.read()
    articles_xml = contents.split("</article>")
    newspaper_articles = []
    invalid_count = 0
    count = 0
    parser = etree.XMLParser(recover=True)
    for article_xml in articles_xml:
        article_xml += "</article>" 
        root = etree.XML(article_xml, parser)
        if root is None:
            invalid_count += 1
            continue
        newspaper_article = Article(root.find("url").text)
        newspaper_article.date = root.find("date").text
        newspaper_article.title = root.find("title").text
        for author in root.find("authors").findall("author"):
            newspaper_article.authors.append(author.text)
        newspaper_article.text = root.find("text").text
        newspaper_articles.append(newspaper_article)
        count += 1
    print(count)
    print(invalid_count)
    return newspaper_articles




