from newspaper import Article
import sys

def xml_str(article):
    result = "<article>"
    result += "<date>" + article.publish_date.strftime("%Y-%m-%d %H:%M:%S") + "</date>"
    result += "<title>" + article.title + "</title>"
    result += "<authors>"
    for author in article.authors:
        result += "<author>" + author + "</author>"
    result += "<authors>"
    result += "</article>"
    return result


def main():
    params = sys.argv[1:]
    assert len(params) == 1
    article = Article(params[0])
    article.download()
    article.parse()
    print(xml_str(article))
    print(article.text)
    load_dictionary()

if __name__ == "__main__":
    main()

