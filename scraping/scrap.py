from newspaper import Article
import sys

def main():
    params = sys.argv[1:]
    assert len(params) == 1
    article = Article(params[0])
    article.download()



if __name__ == "__main__":
    main()

