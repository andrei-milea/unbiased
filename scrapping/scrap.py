import newspaper
from newspaper import Article

def crawl_website(url):
    print('test1')
    source = newspaper.build(url, memoize_articles=False)
    print(len(source.articles))
    for src_article in source.articles:
        src_article.download()
        print('test')
        print(len(tfidf(src_article.text)))
        break

def download_articles(srcs_filename):
    sources_file = open(srcs_filename, 'r')
    sources = sources_file.read().splitlines()
    articles_txt = []
    for source_str in sources:
        source = newspaper.build(source_str, memoize_articles=False)
        print(source_str + " " + str(len(source.articles)))
        for src_article in source.articles:
            src_article.download()
            src_article.parse()
            if(len(src_article.text) > 1000):
                articles_txt.append(src_article.text)
    return articles_txt
