import logging

class Article:
    author_ = ""
    title_ = ""
    date_ = ""
    text_ = ""
    places_ = []
    people_ = []
    orgs_ = []
    topics_ = []

    def __init__(self, author, title, date, text, places, people, orgs, topics):
        self.author_ = author
        self.title_ = title
        self.date_ = date
        self.text_ = text
        self.places_ = places
        self.people_ = people
        self.orgs_ = orgs
        self.topics_ = topics



