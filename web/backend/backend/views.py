from django.http import HttpResponse
from django.template.loader import get_template

def add_link(request):
    add_link_templ = get_template('add_link.html')
    html = add_link_templ.render({'link_type': 'article'})
    return HttpResponse(html)
