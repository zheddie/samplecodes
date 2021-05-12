#!/usr/bin/python
import re
# content='this is html.<img src="%E6%97%A0%E6%A0%87%E9%A2%98%E7%AC%94%E8%AE%B0_96.resources/IMG_20170522_071751.jpg" height="4608" width="3456">we may have other things here <table></table><div>xxxx</div><img src="%E6%97%A0%E6%A0%87%E9%A2%98%E7%AC%94%E8%AE%B0_96.resources/IMG_20170522_071740.jpg" height="3456" width="4608">May other things here. <br><h1></h1><img src="%E6%97%A0%E6%A0%87%E9%A2%98%E7%AC%94%E8%AE%B0_96.resources/IMG_20170522_071553.jpg" height="3456" width="4608"><hr><br>'
content='this is html.<img src="%E6%97%A0%E6%A0%87%E9%A2%98%E7%AC%94%E8%AE%B0_96.resources/IMG_20170522_071751.jpg" height="4608" width="3456">we may have other things here <table></table><div>xxxx</div><img src="http://do.not.change.me.jpg" height="3456" width="4608">May other things here. <br><h1></h1><img src="%E6%97%A0%E6%A0%87%E9%A2%98%E7%AC%94%E8%AE%B0_96.resources/IMG_20170522_071553.jpg" height="3456" width="4608"><hr><br>'

def replaceimg(line):
    stop = False
    begin=0
    end = len(line)
    newline=""
    while(not stop):
        matchedstr = re.search(r'<img src=".*?>',line[begin:end])
        if matchedstr:
            # print(matchedstr.span())
            newline+=line[begin:begin+matchedstr.span()[0]]
            # print("NEWLINE:"+newline)
            matchs = re.match(r'<img src="(.*)\.resources\/(.*?)"',matchedstr.group(),re.M|re.I|re.S)
            if matchs:
                newline+='<img src="'+'REPLACEID'+'.resources/'+matchs.group(2)+'"'+matchedstr.group()[matchs.span()[1]:]
            else:
                newline+=matchedstr.group()
            begin+=matchedstr.span()[1]
        else:
            newline+=line[begin:end]
            stop = True
    return(newline)    

nl = replaceimg(content)
print(nl)