from django.http import HttpResponse
from django.template.loader import get_template
from django.core.validators import URLValidator
from django.core.exceptions import ValidationError
import logging
from django.conf import settings


import pdb

def contribute_page(request):
    html = None
    if request.method == 'GET':
        add_link_templ = get_template('contribute.html')
        html = add_link_templ.render({'story_list': ['CNN', 'FOX', 'MSNBC']})
        logging.getLogger('root').debug("test1")
    else:
        logging.getLogger('root').debug("test2")
        validator = URLValidator()
        print(request)
        try:
            validator(request)
        except ValidationError as err:
            print(err)

        #TODO urlparse, send to c++ code, return success/error page to user
    return HttpResponse(html)

