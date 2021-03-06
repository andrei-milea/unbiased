from django.db import models
from django.contrib.auth.models import User

class UserEntry(models.Model):
    user = models.ManyToManyField(User)
    url = models.URLField(unique=True, blank=True)
    text = models.TextField(blank=True)
    keywords = models.CharField(max_length=200, blank=True)
    processed = models.BooleanField(default=False)

class Story(models.Model):
    title = models.CharField(max_length=200)
    entry = models.ManyToManyField(UserEntry)
    publication_date = models.DateField()



