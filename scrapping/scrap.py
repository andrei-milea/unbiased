import newspaper
from newspaper import Article
import random
from nltk_utils import get_vocabulary
import gc

def crawl_website(url):
    print('test1')
    source = newspaper.build(url, memoize_articles=False)
    print(len(source.articles))
    for src_article in source.articles:
        src_article.download()
        print('test')
        print(len(tfidf(src_article.text)))
        break

def download_articles_sample(srcs_filename):
    sources_file = open(srcs_filename, 'r')
    sources = sources_file.read().splitlines()
    articles = []
    for source_str in sources:
        try:
            source = newspaper.build(source_str, memoize_articles=False, number_threads=1, fetch_images=False)
        except Exception as exp:
            print('build source exception: ' + src_article + ' error: ' + repr(exp))
        print(source_str + " " + str(len(source.articles)))
        count = 0
        random.shuffle(source.articles)
        for src_article in source.articles:
            try:
                src_article.download()
                src_article.parse()
                if(len(src_article.text) > 1000):
                    articles.append(src_article)
                    count += 1
                if count == 100:
                    break;
            except Exception as exp:
                print('download/parse article exception for: ' + source_str + ' error: ' + repr(exp))
    return articles

def get_articles_urls(srcs_filename):
    sources_file = open(srcs_filename, 'r')
    sources = sources_file.read().splitlines()
    articles_urls = []
    for source_str in sources:
        try:
            source = newspaper.build(source_str, memoize_articles=False, number_threads=1, fetch_images=False)
        except Exception as exp:
            print('build source exception: ' + src_article + ' error: ' + repr(exp))
        print(source_str + " " + str(len(source.articles)))
        for src_article in source.articles:
            articles_urls.append(src_article.url)
    return articles_urls

def download_article(url):
    article = Article(url)
    article.download()
    article.parse()
    return article

def scrap_vocabulary(srcs_filename):
    sources_file = open(srcs_filename, 'r')
    sources = sources_file.read().splitlines()
    vocabulary = set()
    for source_str in sources:
        try:
            source = newspaper.build(source_str, memoize_articles=False, fetch_images=False)
        except Exception as exp:
            print('build source exception: ' + src_article + ' error: ' + repr(exp))
        print(source_str + " " + str(len(source.articles)))
        for src_article in source.articles:
            try:
                article = src_article
                article.download()
                article.parse()
                if(len(src_article.text) > 1000):
                    vocabulary = vocabulary.union(get_vocabulary(article.title))
                    vocabulary = vocabulary.union(get_vocabulary(article.text))
                    del article
                    gc.collect()
            except Exception as exp:
                print('download/parse article exception for: ' + source_str + ' error: ' + repr(exp))
    return vocabulary

