#!/usr/bin/env python
import re
import lxml.html
s=""
while True:
    w = raw_input()
    s = s+w
    if w=="ENDFILE":
        break
hxs = lxml.html.document_fromstring(s)
links = hxs.xpath('//a/@href')
for link in links
    w  = link.split('?')[0];
    link = w
print ','.join(links)