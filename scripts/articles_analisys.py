from mongo_utils import MongoUtils
from collections import defaultdict
import matplotlib.pyplot as plt
import pandas

def author_freq_hist():
    auth_freq_map = defaultdict(int)
    mng_db = MongoUtils('test_db')
    articles = mng_db.load_articles_meta()
    for record in articles:
        authors = record['authors']
        for author in authors:
            auth_freq_map[author] += 1
    #for key, val in art_len_map:
    print('number of authors ', len(auth_freq_map))
    df = pandas.DataFrame.from_dict(auth_freq_map, orient='index')
    df.plot(kind='bar')
    plt.show()


def term_doc_mat():
    mng_db = MongoUtils('test_db')
    

def main():
    try:
        author_freq_hist()
    except Exception as exp:
        print('data_analysis caught error: ' + repr(exp))

if __name__ == "__main__":
    main()


