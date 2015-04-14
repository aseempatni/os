#!/usr/bin/env python
import re
import lxml.html
import urlparse
from urlparse import urljoin
def is_absolute(url):
    return bool(urlparse.urlparse(url).netloc)

s=""
while True:
    w = raw_input()
    s = s+w
    if w=="ENDFILE":
        break
hxs = lxml.html.document_fromstring(s)
links = hxs.xpath('//a/@href')
for link in links:
    if not is_absolute(link):
        link = urljoin(s, link)
print ','.join(links)
