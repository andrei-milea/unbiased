from django.http import HttpResponse
from django.template.loader import get_template
from django.core.validators import URLValidator
from django.core.exceptions import ValidationError
import sys
import logging
import re
import pdb

def parse_contribute_req(request):
    print(request.body)
    items = re.split("&", request.body.decode('utf-8'))
    for item in items:
        parts = re.split("=", item)
        if parts[0] == 'src_type_sel':
            src_type = parts[1]
        if parts[0] == 'slink':
            user_input = parts[1]
        if parts[0] == 'stext':
            user_input = parts[1]
        if parts[0] == 'keywords':
            keywords = parts[1]
    return src_type, user_input, keywords
    

def contribute_page(request):
    html = None
    try:
        if request.method == 'GET':
            add_link_templ = get_template('contribute.html')
            html = add_link_templ.render({'story_list': ['CNN', 'FOX', 'MSNBC']})
            #logging.getLogger('root').debug("test1")
        else:
            uinput  = parse_contribute_req(request)
            if uinput[0] != 'Text':
                validator = URLValidator()
                validator(uinput[1])
                user_entry = UserEntry.objects.create(user='test', url=uinput[1], keywords=uintput[2])
            else:
                user_entry = UserEntry.objects.create(user='test', text=uinput[1], keywords=uintput[2])
            user_entry.save()

    except ValidationError as err:
        print(err)
    except:
        print(sys.exc_info()[0])
    return HttpResponse(html)

