from nltk_utils import tfidf
import sys
import nltk

def xml_str(article):
    unbiased_shard = "@unbiased-shard-00-00-5jeo1.mongodb.net:27017,unbiased-shard-00-01-5jeo1.mongodb.net:27017,unbiased-shard-00-02-5jeo1.mongodb.net:27017/test?ssl=true&replicaSet=unbiased-shard-0&authSource=admin"
    mongo_utils = MongoUtils(username = "rashomon", password = "ChangeIt!@34", shard = unbiased_shard)
    vocabulary = mongo_utils.load_vocabulary()
    result = "<article>"
    result += "<date>" + article.publish_date.strftime("%Y-%m-%d %H:%M:%S") + "</date>"
    result += "<title>" + article.title + "</title>"

    result += "<authors>"
    for author in article.authors:
        result += "<author>" + author + "</author>"
    result += "<authors>"

    words = nltk.word_tokenize(article.text)
    result += "<ngrams>" + nltk.trigrams(words) + "</ngrams>"
    result += "<tfidf>" + tfidf(article.text, vocabulary) + "</tfidf>"
    
    result += "</article>"
    return result

def main():
    try:
        params = sys.argv[1:]
        for param in params:
            article = Article(param)
            article.download()
            article.parse()
            print(xml_str(article))
            print(article.text)
    except Exception as exp:
        print('caught error: ' + repr(exp))

if __name__ == "__main__":
    main()



