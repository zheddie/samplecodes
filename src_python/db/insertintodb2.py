#!/usr/bin/env python3


import sys
import os
import re
import ibm_db_dbi as dbi

    #pid,ppid,isacc,TimeToAnswer,score,txt,NumTextTokens, NumCodeLines, LinkCount, NumImages = values
values = (1,-1,0,0,25,b"I am upgrading the internal SATA hard drive on my laptop from a 40G drive to a 160G drive. I have a Linux/Ubuntu desktop which has a SATA card. I would actually like to do the same thing for a couple CentOS &amp; FreeBSD boxes at work, and it seems this would have the same solution.I've heard that I can use DD to mirror the 40G partition to the 160G drive, or that I can save the 40G partition as an image on my local system, and then copy that 40G image to the 160G drive.Can anyone describe how I may do this? Do I need any other utilities, such as gparted",111,0,2,0)
#values = (1,-1,0,0,25,'Hello',111,0,2,0)
pid,ppid,isacc,TimeToAnswer,score,txt,NumTextTokens, NumCodeLines, LinkCount, NumImages = values

#sqlcmd = "insert into qmldemo.fltr values("+",".join(map(str, values))+")"


try:
    conn = dbi.connect()
    cur = conn.cursor()
    #sqlcmd = "insert into qmldemo.fltr values("+str(pid)+","+str(ppid)+","+str(isacc)+","+str(TimeToAnswer)+","+str(score)+",'"+str(txt).replace('\'','\'\'')+"',"+str(NumTextTokens)+","+str( NumCodeLines)+","+str( LinkCount)+","+str( NumImages)+")"
    sqlcmd = "insert into qmldemo.fltr values(420,13,0,75260,0,'On Ubuntu 10.04 you can control some startup programs from the GUI. System?Preferences?Startup Applications',13,0,0,0)"
    print("sqlcmd="+sqlcmd)
    cur.execute(sqlcmd)
    conn.commit()
    sqlcmd = "select * from qmldemo.fltr"
    print("sqlcmd="+sqlcmd)
    cur.execute(sqlcmd)
    for i, row in enumerate(cur, start=1):
    	print(i, row)   
    
except Exception as err:
    print('ERROR:  ' + str(err))
