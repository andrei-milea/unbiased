from nltk import corpus
from nltk.corpus import stopwords
from sklearn.feature_extraction.text import TfidfVectorizer
from nltk.stem import WordNetLemmatizer
from nltk import word_tokenize

def corpus_vocab(self):
        words = corpus.words.words()
        stemmer = PorterStemmer()
        lematizer = WordNetLemmatizer()
        words =  [lematizer.lemmatize(word) for word in words]
        return words

class lemma_tokenizer(object):
    def __init__(self):
        self.lemmatizer = WordNetLemmatizer()
    def __call__(self, text):
        return [self.lemmatizer.lemmatize(t) for t in word_tokenize(text)]

def tfidf(text, dbvocabulary):
        eng_stop_words = set(stopwords.words("english"))
        vectorizer = TfidfVectorizer(analyzer = "word", min_df=1, token_pattern=r'\w{4,}', tokenizer = lemma_tokenizer,
                smooth_idf=True, preprocessor = None, stop_words = eng_stop_words, max_features = 1000000, vocabulary = dbvocabulary)
        term_doc_mat = vectorizer.fit_transform(text)
        return term_doc_mat.toarray().transpose()

def tfidf_docs(docs):
        eng_stop_words = set(stopwords.words("english"))
        vectorizer = TfidfVectorizer(analyzer = "word", min_df=1, token_pattern=r'\w{4,}', tokenizer = lemma_tokenizer,
                smooth_idf=True, preprocessor = None, stop_words = eng_stop_words, max_features = 1000000)
        term_doc_mat = vectorizer.fit_transform(docs)
        return term_doc_mat.toarray().transpose(), vectorizer.get_feature_names()


