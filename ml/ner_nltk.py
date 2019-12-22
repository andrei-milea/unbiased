import os
import sys
from ner import ner_processor
from ner_features import  to_standard_ne
from nltk import pos_tag, chunk, ne_chunk
from nltk.metrics.scores import accuracy

class ner_processor_nltk(ner_processor):
    tokens = []
    targets = []

    def __init__(self):
        super(ner_processor_nltk, self).__init__()

    def extract_data(self):
        self.__get_tokens_targets(self.train_data)
        self.__get_tokens_targets(self.testA_data)
        self.__get_tokens_targets(self.testB_data)

    def __get_tokens_targets(self, input_data):
        for sample in input_data:
            self.tokens.append(sample[0])
            self.targets.append(to_standard_ne(sample[3]))
            # Group NE data into tuples

    def run(self):
        ne_tree = ne_chunk(pos_tag(self.tokens))
        #Convert prediction to multiline string and then to list (includes pos tags)
        multiline_string = chunk.tree2conllstr(ne_tree)
        listed_pos_and_ne = multiline_string.split()

        # Delete pos tags and rename
        del listed_pos_and_ne[1::3]
        listed_ne = listed_pos_and_ne

        # Amend class annotations for consistency with reference_annotations
        for n , entity in enumerate(listed_ne):
            listed_ne[n] = to_standard_ne(entity)
        nltk_formatted_prediction = listed_ne[1 : : 2]
        nltk_accuracy = accuracy(self.targets, nltk_formatted_prediction)
        print('accuracy: {}'.format(nltk_accuracy))

def main():
    try:
        ner = ner_processor_nltk()
        ner.extract_data()
        ner.run()

    except Exception as exp:
        exc_type, exc_obj, exc_tb = sys.exc_info()
        fname = os.path.split(exc_tb.tb_frame.f_code.co_filename)[1]
        print(exc_type, fname, exc_tb.tb_lineno)
 

if __name__ == "__main__":
    main()

