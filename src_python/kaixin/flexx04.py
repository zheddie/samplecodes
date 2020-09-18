# -*- coding: utf-8 -*-

from flexx import flx

class Example(flx.Widget):

    def init(self):
        super().init()
        with flx.VBox():
        	 with flx.HBox():
        	 	self.title = flx.Label(flex=1,text='山东宝帅电力铁芯计算')
            with flx.HBox():
            	self.firstname_lable = flx.Label("输入截面积：")
                self.firstname = flx.LineEdit(placeholder_text='First name')
                self.lastname = flx.LineEdit(placeholder_text='Last name')
            with flx.HBox():
                self.but = flx.Button(text='Reset')
                self.label = flx.Label(flex=1)

    @flx.reaction('firstname.text', 'lastname.text')
    def greet(self, *events):
        self.label.set_text('hi ' + self.firstname.text + ' ' + self.lastname.text)

    @flx.reaction('but.pointer_click')
    def reset(self, *events):
        self.label.set_text('')
app = flx.App(Example)
#app.export('example.html', link=0)  # Export to single file
app.launch('browser')  # show it now in a browser
flx.start()  # enter the mainloop