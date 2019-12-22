import time
import pandas as pd
import numpy as np 
import os
import sys
import matplotlib.pyplot as plt

sys.path.insert(1, '../scripts')
from article import from_xml
from mongo_utils import MongoUtils
from ner import ner_processor
from ner_features import *

from nltk.stem import PorterStemmer
import pandas as pd
import numpy as np 
from sklearn.feature_extraction import DictVectorizer
from sklearn.preprocessing import StandardScaler
from sklearn.linear_model import Perceptron
from sklearn.linear_model import LogisticRegression
from sklearn import svm
from sklearn.tree import DecisionTreeClassifier
from sklearn.neighbors import KNeighborsClassifier
from sklearn.model_selection import train_test_split
from sklearn import preprocessing
from sklearn.decomposition import PCA
from sklearn.decomposition import KernelPCA
from sklearn.metrics import accuracy_score
from sklearn.model_selection import StratifiedKFold
from sklearn.model_selection import validation_curve

class ner_processor_custom(ner_processor):
    __vocabulary = {}
    __train_words = set()

    __X = []
    __Y = np.array([]) 
    __train_samples_no = 0
    __testA_samples_no = 0
    __testB_samples_no = 0

    __mongo_utils = MongoUtils('unbiased')
    __stemmer = PorterStemmer()

    def __init__(self):
        super(ner_processor_custom, self).__init__()

        #load vocabulary
        words = self.__mongo_utils.load_vocabulary()
        idx = 0
        for word in words:
            self.__vocabulary[word] = idx
            idx += 1

    def run_conll_split(self, learning_model):
        print('running on conll split')
        print('Using model ' + str(type(learning_model)))
        train_samples_X = self.__X[0 : self.__train_samples_no, :]
        train_samples_Y = self.__Y[0 : self.__train_samples_no]
        testA_samples_X = self.__X[self.__train_samples_no : self.__train_samples_no + self.__testA_samples_no, :]
        testA_samples_Y = self.__Y[self.__train_samples_no : self.__train_samples_no + self.__testA_samples_no]
        testB_samples_X = self.__X[self.__train_samples_no + self.__testA_samples_no : self.__train_samples_no + self.__testA_samples_no + self.__testB_samples_no, :]
        testB_samples_Y = self.__Y[self.__train_samples_no + self.__testA_samples_no : self.__train_samples_no + self.__testA_samples_no + self.__testB_samples_no]
        learning_model.fit(train_samples_X, train_samples_Y)


        #prediction results on training set
        print('lengths: ' + str(len(train_samples_X)) + ' ' + str(len(train_samples_Y)))
        y_pred_train = learning_model.predict(train_samples_X)
        print('Accuracy for trainig set prediction: %.2f' % accuracy_score(train_samples_Y, y_pred_train))

        #prediction results on test set A
        print('lengths: ' + str(len(testA_samples_X)) + ' ' + str(len(testA_samples_Y)))
        y_pred_testA = learning_model.predict(testA_samples_X)
        print('Accuracy for test set A prediction: %.2f' % accuracy_score(testA_samples_Y, y_pred_testA))

        #prediction results on test set B 
        print('lengths: ' + str(len(testB_samples_X)) + ' ' + str(len(testB_samples_Y)))
        y_pred_testB = learning_model.predict(testB_samples_X)
        print('Accuracy for test set B prediction: %.2f' % accuracy_score(testB_samples_Y, y_pred_testB))


    def run(self, learning_model, tst_size=0.3):
        print('Using model ' + str(type(learning_model)))
        print('lengths: ' + str(len(self.__X)) + ' ' + str(len(self.__Y)))

        X_train, X_test, y_train, y_test = train_test_split(self.__X, self.__Y, test_size = tst_size, random_state=0)
        learning_model.fit(X_train, y_train)

        #prediction results on training set
        y_pred = learning_model.predict(X_test)
        print('Accuracy for test prediction: %.2f' % accuracy_score(y_test, y_pred))

    def run_kfold(self, learning_model, k=10):
        kfolds = StratifiedKFold(n_splits=k, random_state=1).split(self.__X, self.__Y)
        scores = []
        for idx, (train, test) in enumerate(kfolds):
            learning_model.fit(self.__X[train], self.__Y[train])
            y_pred = learning_model.predict(self.__X[test])
            print('Accuracy for kfold %2d test prediction: %.2f' % (idx, accuracy_score(self.__Y[test], y_pred)))

    def validation_curve(self, learning_model, param_name, param_range):
        train_scores, test_scores = validation_curve(estimator=learning_model, X=self.__X, y=self.__Y, param_name=param_name, param_range=param_range, cv=10)
        train_mean = np.mean(train_scores, axis=1)
        train_std = np.std(train_scores, axis=1)
        test_mean = np.mean(test_scores, axis=1)
        test_std = np.std(test_scores, axis=1)
        plt.plot(param_range, train_mean, color='blue', marker='o', markersize=5, label='training accuravcy')
        plt.plot(param_range, test_mean, color='green', linestyle='--', marker='s', markersize=5, label='validation accuravcy')
        plt.grid()
        plt.xscale('log')
        plt.legend(loc='lower right')
        plt.xlabel('Parameter')
        plt.ylabel('Accuracy')
        plt.show()

    def pca_preprocessing(self):
        pca = PCA(n_components=int(len(self.__X[0]) / 2))
        self.__X = pca.fit_transform(self.__X)
        

    def kernel_pca_preprocessing(self):
        kern_pca = KernelPCA(n_components=int(len(self.__X[0]) / 2), kernel='rbf', gamma=15)
        self.__X = kern_pca.fit_transform(self.__X)


    def build_features(self):
        self.__build_features(self.train_data, True)
        self.__train_samples_no = len(self.train_data)
        self.__build_features(self.testA_data, False)
        self.__testA_samples_no = len(self.testA_data)
        self.__build_features(self.testB_data, False)
        self.__testB_samples_no = len(self.testB_data)


    def build_targets(self):
        self.__build_targets(self.train_data)
        self.__build_targets(self.testA_data)
        self.__build_targets(self.testB_data)


    def preprocess_features(self):
        self.__X = self.__vectorize(self.__X)
        self.__X = self.__scale(self.__X)

    def __vectorize(self, features):
        dv = DictVectorizer(sparse=False)
        return dv.fit_transform(features)


    def __scale(self, features):
        sc = StandardScaler()
        sc.fit(features)
        return sc.transform(features)


    # data[0] - POS tag, data[1] - chunk tag, data[2] - case, data[3] - word_id,
    # data[4] - ortographic info, data[5] - before word_id, data[6] - after word_id
    def __build_features(self, data, is_train):
        for idx in range(len(data)):
            self.__X.append({})

            self.__X[idx]['pos'] = data[idx][1]
            self.__X[idx]['chunk'] = data[idx][2]
            
            #case
            self.__X[idx]['case'] = get_case(data[idx][0])

            #word_id
            word_id = self.__vocabulary.get(self.__stemmer.stem(data[idx][0]), -1)
            if is_train and word_id != -1:
                self.__train_words.add(word_id)
                self.__X[idx]['word_id'] = word_id
            else:
                if word_id in self.__train_words:
                    self.__X[idx]['word_id'] = word_id
                else:
                    self.__X[idx]['word_id'] = -1

            #ortographic info
            self.__X[idx]['ort_info'] = get_ort_info(data[idx][0])

            if idx == 0 or idx == len(data) - 1:
                self.__X[idx]['before'] = -1 
                self.__X[idx]['after'] = -1 
                self.__X[idx]['prev_tag']  = -1 
                continue

            #before and after word_id
            word_id = self.__vocabulary.get(self.__stemmer.stem(data[idx-1][0]), -1)
            if word_id in self.__train_words or word_id == -1:
                self.__X[idx]['before'] = word_id
            else:
                self.__X[idx]['before'] = -1
            word_id = self.__vocabulary.get(self.__stemmer.stem(data[idx+1][0]), -1)
            if word_id in self.__train_words or word_id == -1:
                self.__X[idx]['after'] = word_id
            else:
                self.__X[idx]['after'] = -1

            #prev tag
            self.__X[idx]['prev_tag'] = data[idx-1][3]


    def __build_targets(self, data):
        label_enc = preprocessing.LabelEncoder()
        labels = [data[row][3] for row in range(len(data))]
        label_enc.fit(labels)
        label_values = label_enc.transform(labels)
        self.__Y = np.append(self.__Y, label_values)
    
def main():
    try:
        ner = ner_processor_custom()
        ner.build_features()
        ner.build_targets()
        ner.preprocess_features()

        '''
        start_time = time.time()
        ppn = Perceptron(eta0=0.1, random_state=1)
        ner.run(ppn)
        print('running time: ' + str(time.time() - start_time))
        print('\n\n')

        start_time = time.time()
        log_reg = LogisticRegression(random_state=1, multi_class='ovr', solver='newton-cg')
        ner.run(log_reg)
        print('running time: ' + str(time.time() - start_time))
        print('\n\n')

        start_time = time.time()
        lin_svm = svm.LinearSVC()
        ner.run(lin_svm)
        print('running time: ' + str(time.time() - start_time))
        print('\n\n')

        #rbk_svm = svm.SVC(kernel='rbf', random_state=1, gamma=0.2, C=1.0)

        start_time = time.time()
        dtree = DecisionTreeClassifier(criterion='gini', random_state=0)
        ner.run(dtree)
        print('running time: ' + str(time.time() - start_time))
        print('\n\n')

        start_time = time.time()
        knn = KNeighborsClassifier(n_neighbors=10, p=2, metric='minkowski')
        ner.run(knn)
        print('running time: ' + str(time.time() - start_time))
        print('\n\n')

        start_time = time.time()
        log_reg = LogisticRegression(random_state=1, multi_class='ovr', solver='newton-cg')
        ner.run_kfold(log_reg)
        print('running time: ' + str(time.time() - start_time))
        print('\n\n')
        '''

        param_range = [0.001, 0.01, 0.1, 1.0, 10.0, 100.0]
        log_reg = LogisticRegression(multi_class='ovr', solver='newton-cg')
        ner.validation_curve(log_reg, 'C', param_range)
        

        #ner.print_titles()
    except Exception as exp:
        exc_type, exc_obj, exc_tb = sys.exc_info()
        fname = os.path.split(exc_tb.tb_frame.f_code.co_filename)[1]
        print(exc_type, fname, exc_tb.tb_lineno)

if __name__ == "__main__":
    main()

