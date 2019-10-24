#!/usr/bin/env python
# -*- coding:utf-8-*-
import Tkinter,io
from Tkinter import *
import tkFileDialog
import tkColorChooser
from tensorflow.examples.tutorials.mnist import input_data
import tensorflow as tf
import skimage.io as ski_io
from PIL import Image,ImageFilter
import numpy as np

CANVASWIDTH=280
np.set_printoptions(threshold=np.inf,linewidth=200,precision=2)
#定义一个画布类
class MyCanvas:
    def __init__(self,root):
        #i主要用来区分画的图形
        self.i=0
        self.state=''   #表示要画的图形(line,oval,arc,rectangle)
        #self.color='#F9400049F' #画笔颜色，默认黑色
        self.color='black'
        self.root=root
        self.canvas=Tkinter.Canvas(root,width=CANVASWIDTH,height=CANVASWIDTH,bg='white')
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
        # self.canvas.create_line(self.x,self.y-8,event.x,event.y-8,fill='#00000049f',width=20
        #     ,smooth='true'
        #     ,splinesteps=1
        #     #,stipple='gray25'
        #     )
        # self.canvas.create_line(self.x,self.y,event.x,event.y,fill='#000000000',width=15
        #     ,smooth='true'
        #     ,splinesteps=12
        #     #,stipple='gray25'
        #     )
        # self.canvas.create_line(self.x,self.y+6,event.x,event.y+6,fill='#f94000000',width=20
        #     ,smooth='true'
        #     ,splinesteps=1
        #     #,stipple='gray25'
        #     )
        # self.canvas.create_oval(self.x-8,self.y-8,event.x+8,event.y+8,fill=self.color,width=0)
        # self.canvas.create_oval(self.x+8,self.y+8,event.x-8,event.y-8,fill=self.color,width=0)
        # self.canvas.create_oval(self.x-8,event.y+8,event.x+8,self.y-8,fill=self.color,width=0)
        # self.canvas.create_oval(self.x+8,event.y-8,event.x-8,self.y+8,fill=self.color,width=0)
        # self.canvas.create_oval(self.x,self.y,event.x,event.y,fill=self.color,width=0)

        self.x=event.x
        self.y=event.y    
    def setStates(self,state):
        self.state=state
    
    
        
        
   
#定义一个按钮类        
class MyButton:
    def __init__(self,root,canvas,label):
        self.root=root
        self.label=label
        self.canvas=canvas
        if label == 'clear':
            #button的定义
            self.button=Tkinter.Button(root,text=label,command=self.clear)
        elif label == 'load':
            #button的定义
            self.button=Tkinter.Button(root,text=label,command=self.loadfile)
        elif label == 'save':
            self.button=Tkinter.Button(root,text=label,command=self.savefile)
        elif label == 'recog':
            self.button=Tkinter.Button(root,text=label,command=self.recognize)
        self.button.pack()
      

    def clear(self):
        self.canvas.canvas.delete("all")


    def loadfile(self):
        self.canvas.setStates("pan")
        tempfile=tkFileDialog.askopenfilename(title='请选择相应的文件',
                                 filetypes=[('图片','*.jpg *.png'),('All files','*')],
                                 initialdir='h:/')
    
        self.canvas.fileInput(tempfile)
    def savefile(self):
        tkFileDialog.asksaveasfilename(title='保存' ,initialdir='h:/')

class MyRecog:
    def __init__(self,root,canvas):
        self.idx = 1
        self.root=root
        self.canvas=canvas.canvas
        self.sess = tf.Session()
        self.button=Tkinter.Button(root,text='recog',command=self.recognize)
        self.button.pack() 
        self.val = Label(frame3, text='', font =('Verdana',30))
        self.val.pack()
        in_units = 784
        h1_units = 300
        W1 = tf.Variable(tf.truncated_normal([in_units, h1_units], stddev=0.1))
        b1 = tf.Variable(tf.zeros([h1_units]))
        W2 = tf.Variable(tf.zeros([h1_units, 10]))
        b2 = tf.Variable(tf.zeros([10]))

        self.x = tf.placeholder(tf.float32, [None, in_units])
        self.keep_prob = tf.placeholder(tf.float32)

        hidden1 = tf.nn.relu(tf.matmul(self.x, W1) + b1)
        hidden1_drop = tf.nn.dropout(hidden1, self.keep_prob)
        y = tf.nn.softmax(tf.matmul(hidden1_drop, W2) + b2)

        # Define loss and optimizer
        #y_ = tf.placeholder(tf.float32, [None, 10])
        saver = tf.train.Saver()
        saver.restore(self.sess, "model/MLP.ckpt")
        self.recogval = tf.argmax(y, 1);
    def recognize(self):
        self.idx += 1
        ps = self.canvas.postscript(
            colormode="color"
            )
        #data = ski_io.imread("tmp_canvas.eps")
        img_orig = Image.open(io.BytesIO(ps.encode('utf-8')))
        #img = img_orig.filter(ImageFilter.BLUR)
        img = img_orig.filter(ImageFilter.GaussianBlur(radius=2))
        #print img
        dd = np.array(img)
        #print dd
        img_l = img.convert("L")
        #img_l = rgb2gray(img)
        #print img_l
        img_o = img_l.resize([28,28])
        img_o.save('tmp'+str(self.idx)+".bmp")

        data = np.array(img_o)
        #print data

        newdata = np.reshape(data,[1,784])

        xinput = newdata.view('float32')
        full = np.full((1,784),255.0,dtype='float32')
         
        xinput = newdata
        xinput = np.divide(np.subtract(full,xinput),full)
        data_dsp = np.reshape(xinput,[28,28])
        #print data_dsp
        for i in range(28):
            for j in range(28):
                if(data_dsp[i,j] > 0.0):
                    print "{:.2f}".format(data_dsp[i,j]),
                else:
                    print '     ',
            print ''
        print '========'
        self.val['text']=self.sess.run(self.recogval,{self.x:newdata, self.keep_prob: 1.0})
        

def Quit():
    quit()   

#os.chdir(os.path.dirname(os.path.abspath(sys.argv[0])))
#生成容器
root=Tkinter.Tk()
#设置窗体大小
root.geometry('400x280')
root.title('Gift to my daughter, Cathy!')

"""该部分主要是关于菜单"""
#生成菜单栏
menubar=Tkinter.Menu(root)
#生成下拉菜单1   tearoff代表下拉菜单，0代表一级下拉菜单 ；1代表二级下拉菜单
submenu1=Tkinter.Menu(menubar,tearoff=0)
#往submenu1菜单中添加命令,command后面是响应函数,不需要()，否则界面生成就会执行该函数
submenu1.add_command(label='Quit',command=Quit)
menubar.add_cascade(label='Gift',menu=submenu1)
#将Menubar（菜单栏）添加到root容器中
root.config(menu=menubar)

"""该部分主要是窗口组件部分"""
#Frame组件
frame1=Tkinter.Frame(root,width=60,height=CANVASWIDTH,bg='pink')
#pack_propagate(0)表示该frame不会随里面放的组件的大小而改变大小
frame1.pack_propagate(0)
#pack()是将组件放在主容器上,side属性表示将组件要放的位置，有top,left,bottom,right
frame1.pack(side='left')

frame2=Tkinter.Frame(root,width=CANVASWIDTH,height=CANVASWIDTH)
frame2.pack_propagate(0)
frame2.pack(side='left')

frame3=Tkinter.Frame(root,width=60,height=CANVASWIDTH,bg='pink')
frame3.pack_propagate(0)
frame3.pack(side='left')        
        
canvas1=MyCanvas(frame2)
canvas1.setStates("pan")
button1=MyButton(frame1,canvas1,'clear')
button2=MyButton(frame1,canvas1,'load')
button3=MyButton(frame1,canvas1,'save')
recog=MyRecog(frame3,canvas1)




root.mainloop()