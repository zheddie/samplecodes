#!/usr/bin/env python3
# -*- coding: utf-8 -*-

from flexx import flx

class TIEXIN(flx.Widget):
	def init(self):
		super().init()
	
		with flx.VBox():
			with flx.HBox():
				self.l1 = flx.Label(html='<i>山东宝帅电力铁芯计算</i>')
			with flx.HBox():
				self.kd_lbl= flx.Label(html='<i>片型宽度：</i>')
				self.kd = flx.LineEdit(text='0')
				self.kd_unit= flx.Label(html='<i>毫米</i>')
			with flx.HBox():
				self.hd_lbl= flx.Label(html='<i>片型厚度：</i>')
				self.hd = flx.LineEdit(text='0')
				self.hd_unit= flx.Label(html='<i>毫米</i>')
			with flx.HBox():
				self.addb = flx.Button(text='+')
			with flx.HBox():
				self.mj_lbl= flx.Label(html='<i>输入截面积：</i>')
				self.mj = flx.LineEdit(text='0')
				self.mj_unit= flx.Label(html='<i>平方厘米</i>')
			with flx.HBox():
				self.zs_lbl= flx.Label(html='<i>输入匝数：</i>')
				self.zs = flx.LineEdit(text='0')
				self.zs_unit= flx.Label(html='<i> </i>')
			with flx.HBox():
				self.dy_lbl= flx.Label(html='<i>输入电压：</i>')
				self.dy = flx.LineEdit(text='0')
				self.dy_unit= flx.Label(html='<i>V</i>')
			with flx.HBox():
				self.xiaoji = flx.Button(text='小记')
				self.cimi = flx.Label(flex=1)
			with flx.HBox():
				self.xs_lbl= flx.Label(html='<i>系数：</i>')
				self.xs = flx.LineEdit(text='0')
				self.xs_unit= flx.Label(html='<i> </i>')
			with flx.HBox():
				self.zl_lbl= flx.Label(html='<i>重量：</i>')
				self.zl = flx.LineEdit(text='0')
				self.zl_unit= flx.Label(html='<i>吨</i>')
			with flx.HBox():
				self.sh_lbl= flx.Label(html='<i>单位损耗：</i>')
				self.sh = flx.LineEdit(text='0')
				self.xh_unit= flx.Label(html='<i>。。</i>')
			with flx.HBox():
				self.jsp0 = flx.Button(text='计算p0')
				self.p0 = flx.Label(flex=1)
			with flx.HBox():
				self.cz = flx.Button(text='重置')
	# @flx.reaction('firstname.text', 'lastname.text')
	# def greet(self, *events):
	# 	self.label.set_text('hi ' + self.firstname.text + ' ' + self.lastname.text)

	@flx.reaction('addb.pointer_click','xiaoji.pointer_click','jsp0.pointer_click','cz.pointer_click')
	def _button_clicked(self, *events):
		ev = events[-1]
		if(ev.source ==self.addb):
			huizong = str(float(self.kd.text)*float(self.hd.text)/100)
			if(huizong != 'NaN'):
				self.mj.set_text(str(float(huizong)+float(self.mj.text)))
		if(ev.source == self.xiaoji):
			dianya = 400.0
			if(self.dy.text != '0'):
				dianya = float(self.dy.text)
			huizong = str(dianya/1.732*45/float(self.mj.text)/float(self.zs.text))
			if(huizong == 'NaN'):
				huizong = '0'
			self.cimi.set_text('磁密：'+huizong)
		if(ev.source == self.jsp0):
			huizong = str(float(self.xs.text)*float(self.zl.text)*float(self.sh.text))
			if(huizong == 'NaN'):
				huizong = '0'
			self.p0.set_text('P0:'+huizong)
		if(ev.source == self.cz):
			self.kd.set_text('0')
			self.hd.set_text('0')
			self.mj.set_text('0')
			self.zs.set_text('0')
			self.dy.set_text('0')
			self.xs.set_text('0')
			self.zl.set_text('0')
			self.sh.set_text('0')
			self.cimi.set_text('密度：0')
			self.p0.set_text('P0:0')
			
app = flx.App(TIEXIN)
#app.export('tiexin.html', link=0)  # Export to single file
app.launch('browser')  # show it now in a browser
flx.start()  # enter the mainloop