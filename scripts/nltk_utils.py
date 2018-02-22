from nltk import corpus
from nltk.corpus import stopwords
from sklearn.feature_extraction.text import TfidfVectorizer
from sklearn.feature_extraction.text import CountVectorizer
from nltk.stem import WordNetLemmatizer
from nltk import word_tokenize

from scipy import linalg

def print_corpus_vocab(words_filename, stopwords_filename):
    words_outfile = open(words_filename, 'w')
    stopwords_outfile = open(stopwords_filename, 'w')
    stop_words = set(stopwords.words("english"))
    words = []
    for word in corpus.words.words():
        if word not in stop_words and len(word) > 2 and not word[0].isupper():
            words.append(word)
    for word in words:
        words_outfile.write(word + ", ")
    for word in stop_words:
        stopwords_outfile.write(word + ", ")

def lemma_tokenize(text):
    stop_words = set(stopwords.words())
    lemmatizer = WordNetLemmatizer()
    return [lemmatizer.lemmatize(t) for t in word_tokenize(text) if t not in stop_words]

def get_vocabulary(text):
    vectorizer = CountVectorizer(analyzer = "word", token_pattern=r'\w{4,}', tokenizer = lemma_tokenize, preprocessor = None, max_features = 1000000)
    vocabulary_dict, X = vectorizer._count_vocab(text, fixed_vocab = False)
    vocabulary = [key for key, val in vocabulary_dict.items()]
    return set(vocabulary)

#tfidf = tf * idf, where idf = log(total no of docs / doc containing term)
def tfidf(text, dbvocabulary):
    vectorizer = TfidfVectorizer(analyzer = "word", min_df=1, token_pattern=r'\w{4,}', tokenizer = lemma_tokenize,
            smooth_idf=True, preprocessor = None, max_features = 1000000, vocabulary = dbvocabulary)
    term_doc_mat = vectorizer.fit_transform(text)
    return term_doc_mat.toarray().transpose()

def tfidf_docs(docs):
    vectorizer = TfidfVectorizer(analyzer = "word", min_df=1, token_pattern=r'\w{4,}', tokenizer = lemma_tokenize,
            smooth_idf=True, preprocessor = None, max_features = 1000000)
    term_doc_mat = vectorizer.fit_transform(docs)
    return term_doc_mat.toarray().transpose(), vectorizer.get_feature_names()

def run_lsa(term_doc_matrix, features):
    u, sigma, vt = linalg.svd(term_doc_matrix, full_matrices=False)
    #compute concepts - keywords
    u.resize((u.shape[0], 100))
    for col_idx in range(u.shape[1]):
        col = u[:, col_idx]
        sorted_idxs = sorted(range(len(col)), key=lambda k: col[k])
        print("concept start")
        for sidx in sorted_idxs:
                print(features[sidx])
        print("concept end")

def main():
    try:
        print_corpus_vocab("test1", "test2")
    except Exception as exp:
        print('caught error: ' + repr(exp))

if __name__ == "__main__":
    main()




