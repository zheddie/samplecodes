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
files = cs.getoutput('find new/src -type f|grep "\.72F$"')
#print files
filelist = files.split('\n')
idx = 0
for file in filelist:
	pathend = file.rfind('/')
	pathstart= file.find('/')
	pathstr = file[pathstart+1:pathend]
	srcfile = file[0:len(file)-4]
	tgtfile = 'patch/'+srcfile[pathstart+1:]+'.patch'
	linkfile = 'link/'+srcfile[pathstart+1:]
	
	oldfile = 'old/'+srcfile[pathstart+1:]
	cmd = 'mkdir -p old/'+pathstr+';cp '+file + ' '+oldfile
	cpout = runcmd(cmd)
	if cpout != '':
		print cpout	
	diffcmd='diff -U 10 '+oldfile+' '+srcfile 
	diffout = runcmd(diffcmd)
	idx+=1
	#print '\n'+cmd
	if diffout != '':
		
		cmd = 'mkdir -p patch/'+pathstr
		mkdirout = runcmd(cmd)
		if mkdirout != '':
			print mkdirout
		cmd = 'diff -U 10 '+oldfile+' '+srcfile +' >'+tgtfile
		runcmd(cmd)
		#Can not use following codes to generate the patch, as there's an extra "0x0a" at then end of file.
		# text_file = open(tgtfile, "w+")
		# text_file.write("%s" % diffout)
		# text_file.close()
	else:
		print 'no out diff:'+diffcmd