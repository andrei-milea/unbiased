from django.http import HttpResponse
from django.template.loader import get_template
from django.core.validators import URLValidator
from django.core.exceptions import ValidationError
from django.shortcuts import render, redirect
from django.contrib.auth import login, authenticate
from django.contrib.auth.forms import UserCreationForm
from django.contrib.auth.decorators import login_required
from .models import UserEntry
from .models import User
import urllib.parse
import sys
import logging
import re
from subprocess import call

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
    
@login_required
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
                validator(input_url)
                print(request.user)
                user_entry = UserEntry.objects.create(url=input_url, keywords=uinput[2])
                user_entry.save()
                user_entry.user.add(request.user)
            else:
                user_entry = UserEntry.objects.create(user='test', text=uinput[1], keywords=uinput[2])
            #user_entry.save()
            call(["killall", "-s", "USR1", "engine_main"])

    except ValidationError as exc:
        logging.getLogger('root').debug("validation error: " + str(exc))
    except:
        logging.getLogger('root').exception('')
    return HttpResponse(html)

