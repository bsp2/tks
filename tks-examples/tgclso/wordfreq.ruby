#!/usr/bin/ruby
# -*- mode: ruby -*-
# $Id: wordfreq.ruby,v 1.9 2001/05/16 23:46:40 doug Exp $
# http://www.bagley.org/~doug/shootout/

freq = Hash.new(0)
loop {
    data = (STDIN.read(4095) or break) << (STDIN.gets || "")
    for word in data.downcase.tr_s('^A-Za-z',' ').split(' ')
	freq[word] += 1
    end
}
freq.delete("")

lines = Array.new
freq.each{|w,c| lines << sprintf("%7d\t%s\n", c, w) }
print lines.sort.reverse
