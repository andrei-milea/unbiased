import re 

# 0 - is lower case, 1 - is upper case, 2 - is mixed case
def get_case(word):
    if word.islower():
        return 'LOW' 
    if word.isupper():
        return 'UPPER' 
    else:
        return 'MIXED' 


def to_standard_ne(entity):
    if entity == "I-PER" or entity == "B-PERSON" or entity == "I-PERSON":
        return "PERSON"
    if entity == "I-ORG" or entity == "I-ORGANIZATION":
        return "ORGANIZATION"
    if entity == "I-LOC" or entity == "B-LOCATION" or entity == "I-LOCATION" or entity == "B-GPE" or entity == "I-GPE":
        return "LOCATION"
    return entity


def get_ort_info(word):
    if re.match('[0-9]+(\.[0-9]*)?|[0-9]*\.[0-9]+$', word):
        return 'number'
    if re.match('__.+__$', word):
        return 'wildcard'
    if re.match('[A-Za-z0-9]+\.$', word):
        return 'ending-dot'
    if re.match('[A-Za-z0-9]+\.[A-Za-z0-9\.]+\.$', word):
        return 'abbreviation'
    if re.match('[A-Za-z0-9]+\-[A-Za-z0-9\-]+.*$', word):
        return 'contains-hyphen'
    if word.startswith("'") and word.endswith("'"):
        return 'quotes'
    return 'none'

