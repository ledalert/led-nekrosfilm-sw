#!/bin/env python3
#encoding=utf-8
import re, sys

r = re.compile(r'^(.*?):([0-9]+):([0-9]+):\s(.*?):\s(.*?)$')

def atr(string, *col):	
	return '\x1b[%sm%s\x1b[0m' % (';'.join(str(c) for c in col), string)

WT = {
	'warning': (3, 36), 
	'note': (1, 3, 34),
	'error': (1, 3, 31),
}


for line in sys.stdin.readlines():
	found = r.findall(line)
	if len(found) == 1:
		(filename, row, col, wtype, firstline), = found
		try:
			sys.stderr.write( "%s:%s:%s: %s: %s\n" % (atr(filename, 1), row, col, atr(wtype, *WT.get(wtype, 0)) , firstline))
		except:
			sys.stderr.write(atr(line, 3))
	else:
		sys.stderr.write(line)


sys.stderr.flush()
sys.stdout.flush()