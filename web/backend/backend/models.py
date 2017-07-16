from django.db import models
from django.contrib.auth.models import AbstractBaseUser

class User(AbstractBaseUser):
    name = models.CharField(max_length=30, unique=True)
    email = models.EmailField(unique=True)

    def __unicode__(self):
        return self.name

class Link(models.Model):
    user = models.ManyToManyField(User)
    url = models.URLField(unique=True)

class Story(models.Model):
    title = models.CharField(max_length=200)
    links = models.ManyToManyField(Link)
    publication_date = models.DateField()
