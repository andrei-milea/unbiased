from django.http import HttpResponse
from django.template.loader import get_template
from django.core.validators import URLValidator
from django.core.exceptions import ValidationError
from django.shortcuts import render, redirect
from django.contrib.auth import login, authenticate
from django.contrib.auth.forms import UserCreationForm
from .models import UserEntry
from .models import User
import urllib.parse
import sys
import logging
import re
from subprocess import call

#from django.contrib.auth.decorators import login_required
#@login_required
def home(request):
    print("home req")
    return render(request, 'home.html')

def signup(request):
    print("signup req")
    if request.method == 'POST':
        form = UserCreationForm(request.POST)
        if form.is_valid():
            form.save()
            username = form.cleaned_data.get('username')
            raw_password = form.cleaned_data.get('password1')
            user = authenticate(username=username, password=raw_password)
            login(request, user)
            return redirect('home')
    else:
        form = UserCreationForm()
    return render(request, 'signup.html', {'form': form})

def parse_contribute_req(request):
    #print(request.body)
    keywords = ''
    items = re.split("&", request.body.decode('utf-8'))
    for item in items:
        parts = re.split("=", item)
        if parts[0] == 'src_type_sel':
            src_type = parts[1]
        elif parts[0] == 'slink':
            user_input = parts[1]
        elif parts[0] == 'stext':
            user_input = parts[1]
        elif parts[0] == 'keywords':
            keywords = parts[1]
    return src_type, user_input, keywords
    
def add_user(user_name, user_email, user_pass):
    user = User.objects.create(name = user_name, email = user_email)
    user.save()

def contribute_page(request):
    html = None
    try:
        if request.method == 'GET':
            add_link_templ = get_template('contribute.html')
            html = add_link_templ.render({'story_list': ['CNN', 'FOX', 'MSNBC']})
            #logging.getLogger('root').debug("test1")
        else:
            logging.getLogger('root').debug("test1")
            uinput  = parse_contribute_req(request)
            if uinput[0] != 'Text':
                validator = URLValidator()
                input_url = urllib.parse.unquote(uinput[1])
                print(input_url)
                validator(input_url)
                #add_user("test", "test@yahoo.com", "test_pass")
                curr_user = User.objects.get(name='test')
                user_entry = UserEntry.objects.create(url=input_url, keywords=uinput[2])
                user_entry.save()
                user_entry.user.add(curr_user)
            else:
                user_entry = UserEntry.objects.create(user='test', text=uinput[1], keywords=uinput[2])
            #user_entry.save()
            call(["killall", "-s", "USR1", "engine_main"])

    except ValidationError as exc:
        logging.getLogger('root').debug("validation error: " + str(exc))
    except:
        logging.getLogger('root').exception('')
    return HttpResponse(html)

