#!/usr/bin/env python

# The Great Computer Language Shootout
# http://shootout.alioth.debian.org/
#
# modified by Ian Osgood

import sys
import itertools
import bisect

def gen_random(max_, last=[42], ia=3877, ic=29573, im=139968):
    last[0] = (last[0] * ia + ic) % im
    return (max_ * last[0]) / im

def make_cumulative(table):
    l = []
    prob = 0.0
    for char, p in table:
        prob += p
        l.append((prob, char))
    return l

alu = (
   "GGCCGGGCGCGGTGGCTCACGCCTGTAATCCCAGCACTTTGG"
   "GAGGCCGAGGCGGGCGGATCACCTGAGGTCAGGAGTTCGAGA"
   "CCAGCCTGGCCAACATGGTGAAACCCCGTCTCTACTAAAAAT"
   "ACAAAAATTAGCCGGGCGTGGTGGCGCGCGCCTGTAATCCCA"
   "GCTACTCGGGAGGCTGAGGCAGGAGAATCGCTTGAACCCGGG"
   "AGGCGGAGGTTGCAGTGAGCCGAGATCGCGCCACTGCACTCC"
   "AGCCTGGGCGACAGAGCGAGACTCCGTCTCAAAAA"
)

iub = [
    ("a", 0.27),
    ("c", 0.12),
    ("g", 0.12),
    ("t", 0.27),

    ("B", 0.02),
    ("D", 0.02),
    ("H", 0.02),
    ("K", 0.02),
    ("M", 0.02),
    ("N", 0.02),
    ("R", 0.02),
    ("S", 0.02),
    ("V", 0.02),
    ("W", 0.02),
    ("Y", 0.02),
]

homosapiens = [
    ("a", 0.3029549426680),
    ("c", 0.1979883004921),
    ("g", 0.1975473066391),
    ("t", 0.3015094502008),
]


def make_repeat_fasta(id_, desc, src, n):
    print ">%s %s" % (id_, desc)
    width = 60
    l = len(src)
    s = src * (n // l)
    s += src[:n % l]
    assert len(s) == n
    print "\n".join([s[i:i+width] for i in xrange(0, n, width)])

def make_random_fasta(id_, desc, table, n):
    print ">%s %s" % (id_, desc)
    width = 60
    chunk = 1 * width
    _gr = gen_random
    _table = make_cumulative(table)
    probs = [t[0] for t in _table]
    chars = [t[1] for t in _table]
    _bisect = bisect.bisect
    r = range(width)
    for j in xrange(n // width):
        print "".join([chars[_bisect(probs, _gr(1.0))] for i in r])
    if n % width:
        print "".join([chars[_bisect(probs, _gr(1.0))] for i in xrange(n % width)])

def main():
    try:
        n = int(sys.argv[1])
    except:
        print "Usage: %s <N>" % sys.argv[0]
    make_repeat_fasta('ONE', 'Homo sapiens alu', alu, n*2)
    make_random_fasta('TWO', 'IUB ambiguity codes', iub, n*3)
    make_random_fasta('THREE', 'Homo sapiens frequency', homosapiens, n*5)

main()
