#!/usr/bin/env python
# -*- coding: utf-8 -*-
import wx #引入wx模块
app = wx.App() #实例化一个主循环
frame = wx.Frame(None,title='test',size=(300,300)) #实例化一个窗口
frame.Show()#调用窗口展示功能
app.MainLoop()#启动主循环
