import pytest
import sys
import nltk
import string
import re
from nltk.corpus import stopwords

from gensim import corpora
from gensim.corpora.dictionary import Dictionary
from gensim.models import LsiModel
from gensim.models.coherencemodel import CoherenceModel
from gensim.test.utils import common_corpus, common_dictionary
from gensim.models.coherencemodel import CoherenceModel
from gensim.corpora.textcorpus import TextCorpus
from gensim import utils
from gensim.parsing.preprocessing import preprocess_string, strip_punctuation, strip_numeric

from gensim.corpora import WikiCorpus

import term_doc_mat
sys.path.insert(1, '../scripts')
from article import from_xml

def compute_coherence_values(dictionary, doc_term_matrix, doc_clean, stop, start=2, step=3):
    """
    Input   : dictionary : Gensim dictionary
              corpus : Gensim corpus
              texts : List of input texts
              stop : Max num of topics
    purpose : Compute c_v coherence for various number of topics
    Output  : model_list : List of LSA topic models
              coherence_values : Coherence values corresponding to the LDA model with respective number of topics
    """
    coherence_values = []
    model_list = []
    for num_topics in range(start, stop, step):
        # generate LSA model
        model = LsiModel(doc_term_matrix, num_topics=number_of_topics, id2word = dictionary)  # train model
        model_list.append(model)
        coherencemodel = CoherenceModel(model=model, texts=doc_clean, dictionary=dictionary, coherence='c_v')
        coherence_values.append(coherencemodel.get_coherence())
    return model_list, coherence_values

def articles_to_text(articles):
    texts = []
    stop_words = set(stopwords.words('english'))
    table = str.maketrans('', '', string.punctuation)
    for article in articles:
        all_words = re.split(r'\W+', article.text)
        lower_words = [word.lower() for word in all_words if word]
        words = [word for word in lower_words if word not in stop_words]
        stripped = [word.translate(table) for word in words]
        texts.append(stripped)
    return texts

def test_gensim_lsa_concepts():
    articles = from_xml("articles.xml")
    texts = articles_to_text(articles)
    curr_dictionary = Dictionary(texts)
    curr_corpus = [curr_dictionary.doc2bow(text) for text in texts]

    filters = [lambda x: x.lower(), strip_punctuation, strip_numeric]
    for topic_size in range(2, 5):
        for topics_no in range(3, 8):
            lsamodel = LsiModel(curr_corpus, num_topics=topics_no, id2word = curr_dictionary)
            topics_lsi = lsamodel.print_topics(num_topics=topics_no, num_words=topic_size)
            computed_topics = []
            for topic_list_lsi in topics_lsi:
                computed_topics.append(preprocess_string(topic_list_lsi[1], filters))
            cm = CoherenceModel(topics=computed_topics, corpus=curr_corpus, dictionary=curr_dictionary, coherence='u_mass')
            coherence = cm.get_coherence()
            print('umass coherence score {} for {} topics with size {}'.format(coherence, topics_no, topic_size))
            

def test_lsa_concepts():
    articles = from_xml("articles.xml")
    texts = articles_to_text(articles)
    curr_dictionary = Dictionary(texts)
    curr_corpus = [curr_dictionary.doc2bow(text) for text in texts]
    for topic_size in range(2, 5):
        for topics_no in range(3, 8):
            computed_topics = term_doc_mat.compute_topics(topics_no, topic_size)
            cm = CoherenceModel(topics=computed_topics, corpus=curr_corpus, dictionary=curr_dictionary, coherence='u_mass')
            coherence = cm.get_coherence()
            print('umass coherence score {} for {} topics with size {}'.format(coherence, topics_no, topic_size))


def test_gensim_one():
    topics_no = 10 
    topic_size = 5
    articles = from_xml("articles.xml")
    texts = articles_to_text(articles)
    curr_dictionary = Dictionary(texts)
    curr_corpus = [curr_dictionary.doc2bow(text) for text in texts]

    filters = [lambda x: x.lower(), strip_punctuation, strip_numeric]
    lsamodel = LsiModel(curr_corpus, num_topics=topics_no, id2word = curr_dictionary)
    topics_lsi = lsamodel.print_topics(num_topics=topics_no, num_words=topic_size)
    computed_topics = []
    for topic_list_lsi in topics_lsi:
        computed_topics.append(preprocess_string(topic_list_lsi[1], filters))

    for topic in computed_topics:
        print('topic:')
        for word in topic:
            print('word {}'.format(word))

    cm_umass = CoherenceModel(topics=computed_topics, corpus=curr_corpus, dictionary=curr_dictionary, coherence='u_mass')
    print('umass coherence score {} for {} topics with size {}'.format(cm_umass.get_coherence(), topics_no, topic_size))

    #wiki = WikiCorpus('/home/proj/unbiased/data/enwiki-latest-pages-articles.xml.bz2', dictionary=curr_dictionary)
    #wiki_texts = wiki.sample_texts(100, 5, 10)
    #texts.append(wiki_texts)
    #cm_uci = CoherenceModel(topics=computed_topics, texts = texts, dictionary=curr_dictionary, coherence='c_uci')
    #print('umass coherence score u_mass {}, c_uci {},  for {} topics with size {}'.format(cm_umass.get_coherence(), cm_uci.get_coherence(), topics_no, topic_size))
 

def test_lsa_one():
    topics_no = 10 
    topic_size = 7 
    articles = from_xml("articles.xml")
    texts = articles_to_text(articles)
    curr_dictionary = Dictionary(texts)
    curr_corpus = [curr_dictionary.doc2bow(text) for text in texts]
    computed_topics = term_doc_mat.compute_topics(topics_no, topic_size)

    for topic in computed_topics:
        print('topic:')
        for word in topic:
            print('word {}'.format(word))

    cm_umass = CoherenceModel(topics=computed_topics, corpus=curr_corpus, dictionary=curr_dictionary, coherence='u_mass')
    cm_uci = CoherenceModel(topics=computed_topics, texts = texts, dictionary=curr_dictionary, coherence='c_uci')
    print('umass coherence score u_mass {}, c_uci {},  for {} topics with size {}'.format(cm_umass.get_coherence(), cm_uci.get_coherence(), topics_no, topic_size))

