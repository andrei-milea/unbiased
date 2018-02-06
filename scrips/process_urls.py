import sys
from newspaper import Article

def xml_str(article):
    result = "<article>"
    result += "<url>" + article.url + "</url>"
    result += "<date>" + article.publish_date.strftime("%Y-%m-%d %H:%M:%S") + "</date>"
    result += "<title>" + article.title + "</title>"
    result += "<authors>"
    for author in article.authors:
        result += "<author>" + author + "</author>"
    result += "<authors>"
    result += "<text>" + article.text + "</text>"
    result += "</article>"
    return result

def main():
    try:
        params = sys.argv[1:]
        assert len(params) == 1
        article = Article(params[0])
        article.download()
        article.parse()
        print(xml_str(article))
    except Exception as exp:
        print('caught error: ' + repr(exp))

if __name__ == "__main__":
    main()



