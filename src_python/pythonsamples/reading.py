#!/usr/bin/python
import os,re


def handleCode(code):
	#print "code=",code;
	printed = 0;
	matchObj = re.match( r'(.*|\n*)\/\*(.*?)\*\/( *|\n*)', code, re.S);
	comments = "";
	if (matchObj is not None):
		comments = matchObj.group(2);
		code = matchObj.group(1);
	while (code != ""):
		matchObj = re.match( r'(.*?|\n*?)(\w+?)\((.*?)\) *;(.*|\n*)', code, re.S);
		if (matchObj is not None):
			print "Functions called: ",matchObj.group(2),;
			if (matchObj.group(2) == "printf" or matchObj.group(2) == "return"):
				print " : ",matchObj.group(3);
			else:
				print "";
			printed = 1;
		else:
			break;
		code = matchObj.group(4);
	#print "Code:",code;
	if (printed == 1):
		print "-------------------------";
	if(comments != ""):
		print "Description:\n/*",comments,"*/";
	return;

fp = open("hello.c","r+");

content = fp.read();

#process the header files
print "=====Header section====="
while(content != ""):
	#print content;
	matchObj = re.match( r'(.*?|\n*?)#include ["<](.*?)[">](.*|\n*)', content, re.S);
	if (matchObj is not None):
		# print "========================================";
		# print "1:\n",matchObj.group(1),"\n2:\n",matchObj.group(2),"\n3:\n",matchObj.group(3);

		#print "matchObj.group(1) : ", matchObj.group(1);
		if (matchObj.group(2) == "stdio.h"):
			print "I/O functions declared in this module.";
		elif (matchObj.group(2) == "unistd.h"):
			print "Unix standard functions declared in this module.";
		else:
			print matchObj.group(2),"is included.";
		content = matchObj.group(3);
		
	else:
		break;
#process functions
print "=====Function Sections====="

while(content != ""):
	#print content;
	matchObj = re.match( r'(.*?|\n*?)(\w*?)( +|\n+)(\w+?)\((.*?)\) *{(.*|\n*)', content, re.S);
	if (matchObj is not None):
		# print "========================================";
		# print "1:\n",matchObj.group(1), \
		# 	"2:\n",matchObj.group(2), \
		# 	"3:\n",matchObj.group(3), \
		# 	"4:\n",matchObj.group(4), \
		# 	"5:\n",matchObj.group(5), \
		# 	"6:\n",matchObj.group(6) \
		# 	;
		
		handleCode(matchObj.group(1));
		print "Function:",matchObj.group(4) \
			,"\nReturn:",matchObj.group(2) \
			,"\nParams:",matchObj.group(5) \
			;
		
		content = matchObj.group(6);
		
	else:
		break;
#The code of the last function.
handleCode(content);
fp.close();



