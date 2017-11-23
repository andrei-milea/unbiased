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
        try:
            source = newspaper.build(source_str, memoize_articles=False)
        except Exception as exp:
            print('build source exception: ' + src_article + ' error: ' + repr(exp))
        print(source_str + " " + str(len(source.articles)))
        for src_article in source.articles:
            try:
                src_article.download()
                src_article.parse()
                if(len(src_article.text) > 1000):
                    articles_txt.append(src_article.text)
                    break
            except Exception as exp:
                print('download/parse article exception for: ' + source_str + ' error: ' + repr(exp))
    return articles_txt
