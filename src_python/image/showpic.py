#coding=utf-8
from Tkinter import *
from PIL import Image,ImageTk
def change():
    label.configure(image = bm2)
top = Tk()
s=r'2.jpg'
im=Image.open(s)
s=r'3.jpg'
im2=Image.open(s)
bm = ImageTk.PhotoImage(im)
bm2= ImageTk.PhotoImage(im2)

#bm = PhotoImage(file = "2.png")
#bm2 = PhotoImage(file = "3.png")
label = Label(top, image = bm)
label.pack()
button = Button(top, text = "changepicture", command = change)
button.pack()
top.mainloop()
