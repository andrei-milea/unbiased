import sys
from newspaper import Article
from articles import article_data

def main():
    try:
        params = sys.argv[1:]
        assert len(params) == 1
        article = Article(params[0])
        article.download()
        article.parse()
        print(to_xml(article))
    except Exception as exp:
        print('caught error: ' + repr(exp))

if __name__ == "__main__":
    main()



