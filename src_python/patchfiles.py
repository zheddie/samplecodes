#!/usr/bin/python
import commands as cs
debug=0
def runcmd(cmd):
	if debug == 1:
		print cmd
		out=''
	else:
		out = cs.getoutput(cmd)
	return out
files = cs.getoutput('find patch/src -type f')
#print files
filelist = files.split('\n')
idx = 0
for file in filelist:
	pathend = file.rfind('/')
	pathstart= file.find('/')
	pathstr = file[pathstart+1:pathend]
	tgtfile = file[pathstart+1:len(file)-6]
	
	linkfile = 'link/'+tgtfile
	cmd = 'mkdir -p '+pathstr+';cp '+linkfile+ ' '+tgtfile+';patch -p1 '+tgtfile + ' '+file
	patchout = runcmd(cmd)
	if patchout != '':
		print patchout
