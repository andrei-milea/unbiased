import os
import csv
import io
import logging
import pandas as pd
import matplotlib.pyplot as plt
import numpy as np
import nltk
from nltk.corpus import stopwords
import statsmodels.api as sm

from sklearn.feature_extraction.text import CountVectorizer
from sklearn.feature_extraction.text import TfidfVectorizer

from sklearn.feature_selection import VarianceThreshold
from sklearn.feature_selection import SelectKBest
from sklearn.feature_selection import chi2
from sklearn.feature_selection import SelectFromModel

from sklearn.svm import LinearSVC
from sklearn.cross_validation import train_test_split
from sklearn.linear_model import LogisticRegression
from sklearn.discriminant_analysis import LinearDiscriminantAnalysis
from sklearn.svm import SVC
from sklearn.tree import DecisionTreeClassifier

from sklearn.metrics import accuracy_score
from sklearn.metrics import confusion_matrix
from sklearn.metrics import roc_auc_score
from sklearn.metrics import r2_score

nltk.download()


class YoutubeData:
    """manipulation, extraction and training for youtube data"""
    comments_ = []
    tags_ = []
    train_data_features_ = None
    y_test_ = None
    y_pred_ = None

    def __init__(self, path):
        if os.path.isdir(path):
            for filename in os.listdir(path): 
                if filename.find("csv") == -1:
                    continue
                with open(path+ "/" + filename, 'r', encoding='utf8') as in_file:
                    file_str = in_file.read()
                    file_str.replace(r'\ufeff','')
                    str_io = io.StringIO(file_str)
                    csvreader = csv.reader(str_io)
                    next(csvreader, None)  # skip the header
                    for comment_line in csvreader:
                        self.comments_.append(comment_line[3])
                        self.tags_.append(int(comment_line[4]))
        #logging.warning(len(self.comments_))

    def create_features(self):
        eng_stop_words = set(stopwords.words("english"))
        vectorizer = CountVectorizer(analyzer = "word", tokenizer = None, preprocessor = None, stop_words = eng_stop_words, max_features = 5000)
        #vectorizer = TfidfVectorizer(analyzer = "word", tokenizer = None, preprocessor = None, stop_words = eng_stop_words, max_features = 3000)
        self.train_data_features_ = vectorizer.fit_transform(self.comments_)
        #logging.warning(vectorizer.get_feature_names())
        self.train_data_features_ = self.train_data_features_.toarray()
        #print(pd.notnull(self.train_data_features_))
        #print(self.train_data_features_.shape)

    def clean_data(self):
        #remove features with low variance
        sel = VarianceThreshold()
        self.train_data_features_ = sel.fit_transform(self.train_data_features_)

        #remove worst 1000 features using chi squared test
        self.train_data_features_ = SelectKBest(chi2, k=len(self.train_data_features_[0]) - 1000).fit_transform(self.train_data_features_, self.tags_)

        #recursive feature selection
        lin_reg = LogisticRegression(C=200.0, random_state=0).fit(self.train_data_features_, self.tags_)
        model = SelectFromModel(lin_reg, prefit=True)
        self.train_data_features_ = model.transform(self.train_data_features_)


    def train_lr(self):
        print("training logistic regression")
        X_train, X_test, y_train, self.y_test_ = train_test_split(self.train_data_features_, self.tags_, test_size=0.25, random_state=42)
        classifier = LogisticRegression(C=100.0, random_state=0)
        classifier.fit(X_train, y_train)
        self.y_pred_ = classifier.predict(X_test)


    def train_lda(self):
        print("training lda")
        X_train, X_test, y_train, self.y_test_ = train_test_split(self.train_data_features_, self.tags_, test_size=0.25, random_state=42)
        classifier = LinearDiscriminantAnalysis()
        classifier.fit(X_train, y_train)
        self.y_pred_ = classifier.predict(X_test)


    def train_svm(self):
        print("training svm")
        X_train, X_test, y_train, self.y_test_ = train_test_split(self.train_data_features_, self.tags_, test_size=0.25, random_state=42)
        classifier = SVC(kernel='linear', C=1.0, random_state=0)
        classifier.fit(X_train, y_train)
        self.y_pred_ = classifier.predict(X_test)
      
     
    def train_trees(self):
        print("training trees")
        X_train, X_test, y_train, self.y_test_ = train_test_split(self.train_data_features_, self.tags_, test_size=0.25, random_state=42)
        classifier = DecisionTreeClassifier(criterion='entropy', max_depth=3, random_state=0)
        classifier.fit(X_train, y_train)
        self.y_pred_ = classifier.predict(X_test)


    def print_results(self):
        missclassified = (self.y_pred_ != self.y_test_).sum()
        conf_mat = confusion_matrix(self.y_test_, self.y_pred_)
        roc_auc = roc_auc_score(self.y_test_, self.y_pred_)
        print("true negatives: " + str(conf_mat[0][0]))
        print("false negatives: " + str(conf_mat[1][0]))
        print("true positives: " + str(conf_mat[1][1]))
        print("false positives: " + str(conf_mat[0][1]))
        print("missclassified: " + str(missclassified) + " from: " + str(len(self.y_test_)))
        print('Accuracy: %.2f' % accuracy_score(self.y_test_, self.y_pred_, normalize = False))
        print('ROC AUC: %.2f' % roc_auc)
        print("r2 score: " + str(r2_score(self.y_test_, self.y_pred_)))
        

    def analize(self):
        X_train, X_test, y_train, y_test = train_test_split(self.train_data_features_, self.tags_, test_size=0.25, random_state=42)
        #print(np.linalg.matrix_rank(X_train))
        logit = sm.Logit(y_train, X_train, missing='drop')
        res = logit.fit(method='nm')
        #print(res.params)
        #print(res.summary())
        #res.resid()
        #plot correlation
        #corr_matrix = np.corrcoef(hie_data.data.T)
        #smg.plot_corr(corr_matrix, xnames=hie_data.names)
        #plt.show()



def main():
    reuters_data = YoutubeData("../data/youtube")
    reuters_data.create_features()
    #reuters_data.clean_data()
    print("feature size: " + str(len(reuters_data.train_data_features_[0])))
    reuters_data.train_lr()
    #reuters_data.train_lda()
    #reuters_data.train_svm()
    #reuters_data.train_trees()
    reuters_data.print_results()

if __name__ == "__main__":
    main()
 
