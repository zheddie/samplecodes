#!/usr/bin/env python
# -*- coding:utf-8-*-
import Tkinter
import tkFileDialog
import tkColorChooser


#定义一个画布类
class MyCanvas:
    def __init__(self,root):
        #i主要用来区分画的图形
        self.i=0
        self.state=''   #表示要画的图形(line,oval,arc,rectangle)
        self.color='black' #画笔颜色，默认黑色
        self.root=root
        self.canvas=Tkinter.Canvas(root,width=620,height=500,bg='white')
        self.canvas.pack()
        self.canvas.bind('<Button-1>',self.Press)
        self.canvas.bind('<B1-Motion>',self.Draw)
        
    #press函数表示当鼠标在画布上按下时初始化鼠标按下位置
    def Press(self,event):
        #每次按下鼠标都会i+1,因此按下鼠标画下的图形都会有为一个标记
        self.i+=1
        self.x=event.x
        self.y=event.y
        
   
    #Draw函数表示鼠标松开时，画出图形            
    def Draw(self,event):
        #铅笔
            if self.state=='pan':
                self.canvas.create_line(self.x,self.y,event.x,event.y,fill=self.color)
                self.x=event.x
                self.y=event.y
                
        #画直线
            if self.state =='line':
                #如果找不到tag为line的组件，则生成tag为line的组件
                if not self.canvas.find_withtag('line'+str(self.i)):
                    self.canvas.create_line(self.x,self.y,event.x,event.y,fill=self.color,tags='line'+str(self.i))
                #如果找到了tag为line的组件，则首先删除该组件，再重新绘制图形
                else:
                    self.canvas.delete('line'+str(self.i))
                    self.canvas.create_line(self.x,self.y,event.x,event.y,fill=self.color,tags='line'+str(self.i))
              
                """ #另一种方法：
                if not self.canvas.find_withtag('line'+str(self.i)):
                    self.item=self.canvas.create_line(self.x,self.y,event.x,event.y,tags='line'+str(self.i))
                else:
                    self.canvas.coords(self.item,(self.x,self.y,event.x,event.y))
                """
        #画椭圆                     
            if self.state == 'oval':
                #如果找不到tag为line的组件，则生成tag为line的组件
                if not self.canvas.find_withtag('line'+str(self.i)):
                    self.canvas.create_oval(self.x,self.y,event.x,event.y,outline=self.color,tags='line'+str(self.i))
                #如果找到了tag为line的组件，则首先删除该组件，再重新绘制图形
                else:
                    self.canvas.delete('line'+str(self.i))
                    self.canvas.create_oval(self.x,self.y,event.x,event.y,outline=self.color,tags='line'+str(self.i))
                
        #画圆弧                     
            if self.state == 'arc':
                #如果找不到tag为line的组件，则生成tag为line的组件
                if not self.canvas.find_withtag('line'+str(self.i)):
                    self.canvas.create_arc(self.x,self.y,event.x,event.y,tags='line'+str(self.i))
                #如果找到了tag为line的组件，则首先删除该组件，再重新绘制图形
                else:
                    self.canvas.delete('line'+str(self.i))
                    self.canvas.create_arc(self.x,self.y,event.x,event.y,start=0,tags='line'+str(self.i))
                    
        #画矩形                   
            if self.state == 'rectangle':
                #如果找不到tag为line的组件，则生成tag为line的组件
                if not self.canvas.find_withtag('line'+str(self.i)):
                    self.canvas.create_rectangle(self.x,self.y,event.x,event.y,outline=self.color,tags='line'+str(self.i))
                #如果找到了tag为line的组件，则首先删除该组件，再重新绘制图形
                else:
                    self.canvas.delete('line'+str(self.i))
                    self.canvas.create_rectangle(self.x,self.y,event.x,event.y,outline=self.color,tags='line'+str(self.i))
                
    def setStates(self,state):
        self.state=state
    
    
        
        
   
#定义一个按钮类        
class MyButton:
    def __init__(self,root,canvas,label):
        self.root=root
        self.label=label
        self.canvas=canvas
        if label == '铅笔':
            #button的定义
            self.button=Tkinter.Button(root,text=label,command=self.pan)
        elif label == '直线':
            #button的定义
            self.button=Tkinter.Button(root,text=label,command=self.draw_line)
        elif label == '圆形':
            self.button=Tkinter.Button(root,text=label,command=self.draw_oval)
        elif label == '圆弧':
            self.button=Tkinter.Button(root,text=label,command=self.draw_arc)
        elif label == '矩形':
            self.button=Tkinter.Button(root,text=label,command=self.draw_rectangle)
        elif label == '画笔颜色':
            self.button=Tkinter.Button(root,text=label,command=self.chooseColor)
        self.button.pack()
        
    def pan(self):
        self.canvas.setStates("pan")
    def draw_line(self):
        self.canvas.setStates("line")
    def draw_oval(self):
        self.canvas.setStates("oval")
    def draw_arc(self):
        self.canvas.setStates("arc")
    def draw_rectangle(self):
        self.canvas.setStates("rectangle")
    def chooseColor(self):
        r=tkColorChooser.askcolor()
        self.canvas.color=r[1]




#菜单绑定函数
#打开文件对话框
"""格式：tkFileDialog.askopenfilename(title,filetypes,initialdir,initialfile)
    其中参数为：title:指定对话框标题          filetypes：指定要选择文件的类型  
           initialdir:指定的默认目录    initialfile：指定的默认文件
    函数的返回类型则是该文件的路径名。比如：H:/3.jpg
"""
def FileOpen():
    tempfile=tkFileDialog.askopenfilename(title='请选择相应的文件',
                                 filetypes=[('图片','*.jpg *.png'),('All files','*')],
                                 initialdir='h:/')
    
    global canvas1
    canvas1.fileInput(tempfile)
    
#保存文件对话框
"""函数名：tkFileDialog.asksaveasfilename(title,filetypes,initialdir,initialfile)
    参数如上函数参数
    函数返回类型为：保存文件路径
"""
def FileSave():
    tkFileDialog.asksaveasfilename(title='保存' ,initialdir='h:/')
    
    
    
#生成容器
root=Tkinter.Tk()
#设置窗体大小
root.geometry('700x500')

"""该部分主要是关于菜单"""
#生成菜单栏
menubar=Tkinter.Menu(root)
#生成下拉菜单1   tearoff代表下拉菜单，0代表一级下拉菜单 ；1代表二级下拉菜单
submenu1=Tkinter.Menu(menubar,tearoff=0)
#往submenu1菜单中添加命令,command后面是响应函数,不需要()，否则界面生成就会执行该函数
submenu1.add_command(label='Open',command=FileOpen)
submenu1.add_command(label='Save',command=FileSave)
#submenu1.add_command(label='Close',command=FileClose)
#将submenu1菜单添加到menubar菜单栏中
menubar.add_cascade(label='File',menu=submenu1)
#将Menubar（菜单栏）添加到root容器中
root.config(menu=menubar)

"""该部分主要是窗口组件部分"""
#Frame组件
frame1=Tkinter.Frame(root,width=80,height=500,bg='pink')
#pack_propagate(0)表示该frame不会随里面放的组件的大小而改变大小
frame1.pack_propagate(0)
#pack()是将组件放在主容器上,side属性表示将组件要放的位置，有top,left,bottom,right
frame1.pack(side='left')

frame2=Tkinter.Frame(root,width=620,height=500)
frame2.pack_propagate(0)
frame2.pack(side='right')


        
        
canvas1=MyCanvas(frame2)
button1=MyButton(frame1,canvas1,'铅笔')
button1=MyButton(frame1,canvas1,'直线')
button2=MyButton(frame1,canvas1,'圆形')
button2=MyButton(frame1,canvas1,'圆弧')
button2=MyButton(frame1,canvas1,'矩形')
button2=MyButton(frame1,canvas1,'画笔颜色')



root.mainloop()