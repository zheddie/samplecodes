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

dirs = cs.getoutput('ls -ld *|grep "^d"|awk \'{print $9}\'')
#print files
dirlist = dirs.split('\n')
idx = 0
for onedir in dirlist:
	cmd = 'find '+onedir+'|xargs ls -ld|grep -v \"^d\"'
	cout = runcmd(cmd)
	if cout == '':
		print "empty?"+onedir
		cmd = 'find '+onedir+" -type f;find "+onedir+" -type l"
		pout = runcmd(cmd)
		print pout
		cmd = 'rm -rf '+onedir
		pout = runcmd(cmd)
		
	else:
		print "*********************keep:"+onedir