from django.http import HttpResponse
from django.template.loader import get_template

def contribute_page(request):
    add_link_templ = get_template('contribute.html')
    html = add_link_templ.render({'story_list': ['CNN', 'FOX', 'MSNBC']})
    return HttpResponse(html)

def add_link(request):
    #TODO urlparse, send to c++ code, return success/error page to user
    return HttpResponse()
