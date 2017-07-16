from django.http import HttpResponse
from django.template.loader import get_template
from django.core.validators import URLValidator
from django.core.exceptions import ValidationError

def contribute_page(request):
    add_link_templ = get_template('contribute.html')
    html = add_link_templ.render({'story_list': ['CNN', 'FOX', 'MSNBC']})
    return HttpResponse(html)

def add_link(request):
    validator = URLValidator(verify_exists=True)
    try:
        validator(request)
    except ValidationError as err:
        print(err)
    #TODO urlparse, send to c++ code, return success/error page to user
    return HttpResponse()
