#!/usr/bin/python
import commands as cs
import os.path

debug=0
def runcmd(cmd):
	if debug == 1:
		print cmd
		out=''
	else:
		out = cs.getoutput(cmd)
	return out

files = cs.getoutput('find src -type f')
#print files
if files != '':
	cmd = 'mkdir new;ln -s ../src new/src;mkdir old;ln -s ../link/src old/src'
	runcmd(cmd)

	filelist = files.split('\n')
	for file in filelist:
		pathend = file.rfind('/')
		tgtfile = 'patch/'+file+'.patch'
		
		oldfile = 'old/'+file
		newfile = 'new/'+file
		if os.path.exists(oldfile):
			diffcmd='diff -U 10 '+oldfile+' '+newfile 
			diffout = runcmd(diffcmd)
			if diffout != '':
				cmd = 'mkdir -p patch/'+file[0:pathend]
				mkdirout = runcmd(cmd)
				if mkdirout != '':
					print mkdirout
				cmd = 'diff -U 10 '+oldfile+' '+newfile +' >'+tgtfile
				runcmd(cmd)
			else:
				print 'no out diff:'+diffcmd
		else:
			print 'ERROR? File not exist in link:'+file
	cmd = 'rm -rf new old'
	runcmd(cmd)