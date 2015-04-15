#!/usr/bin/env python
import re
import lxml.html
import urlparse
import sys
from urlparse import urljoin

#def warning(objs):
    #sys.stderr.write("Incomplete : "+objs+"\n")


def is_absolute(url):
    return bool(urlparse.urlparse(url).netloc)

base_url=raw_input()
s=""
while True:
    w = raw_input()
    s = s+w
    if w=="ENDFILE":
        break

hxs = lxml.html.document_fromstring(s)
links = hxs.xpath('//a/@href')
finallist=[]
for link in links:
    if not is_absolute(link):
        link = urljoin(base_url,link)
        #warning(link)
    finallist.append(link)

print ','.join(finallist)
