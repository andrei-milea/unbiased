from nltk import corpus
from nltk.corpus import stopwords
from sklearn.feature_extraction.text import TfidfVectorizer
from nltk.stem import WordNetLemmatizer
from nltk import word_tokenize

from scipy import linalg

def corpus_vocab():
    eng_stop_words = set(stopwords.words("english"))
    words = corpus.words.words()
    lematizer = WordNetLemmatizer()
    words =  [lematizer.lemmatize(word) for word in words if word not in eng_stop_words]
    return words

def lemma_tokenize(text):
    eng_stop_words = set(stopwords.words())
    lemmatizer = WordNetLemmatizer()
    return [lemmatizer.lemmatize(t) for t in word_tokenize(text) if t not in eng_stop_words]

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

def run_lsa(term_doc_matrix):
    u, sigma, vt = linalg.svd(term_doc_matrix)

