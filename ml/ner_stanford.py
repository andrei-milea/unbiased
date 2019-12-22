import os
import sys
from ner_nltk import ner_processor_nltk
from nltk.metrics.scores import accuracy
from nltk.tag.stanford import StanfordNERTagger

class ner_processor_stanford(ner_processor_nltk):
    def __init__(self):
        super(ner_processor_stanford, self).__init__()


    def run(self):
        stanford_ner = StanfordNERTagger('ner/stanford-ner/classifiers/english.all.3class.distsim.crf.ser.gz',
					 'ner/stanford-ner/stanford-ner.jar', encoding='utf-8')
        predicted_tags = stanford_ner.tag(self.tokens)
        predicted_tags = [elem[1] for elem in predicted_tags] 
        stanford_accuracy = accuracy(self.targets, predicted_tags)
        print(stanford_accuracy)

def main():
    try:
        ner = ner_processor_stanford()
        ner.extract_data()
        ner.run()

    except Exception as exp:
        exc_type, exc_obj, exc_tb = sys.exc_info()
        fname = os.path.split(exc_tb.tb_frame.f_code.co_filename)[1]
        print(exc_type, fname, exc_tb.tb_lineno)
 

if __name__ == "__main__":
    main()

