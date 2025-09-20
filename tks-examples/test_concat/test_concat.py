#!/usr/bin/python

import time


s1 = "hallo, "
s2 = "welt..."
s3 = "some "
s4 = "string"
s5 = "concatenations"

r = ""

t = time.time()
l = 0;

for i in range(10000000):
  r = s1 + s2 + s3 + s4 + s5;
  l = len(r);

print "l=" + str(l);
print str(1000*(time.time()-t)) + "ms";




