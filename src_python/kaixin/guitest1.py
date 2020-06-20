#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import wx
class LoginFrame(wx.Frame):
    """
    登录窗口
    """
    def __init__(self, parent, id, title, size):
        # 初始化，添加控件并绑定事件
        wx.Frame.__init__(self, parent, id, title)
        self.SetSize(size)#自动调节窗口大小
        self.Center()#设置登录弹窗在桌面中心
        self.serverAddressLabel = wx.StaticText(self, label="Server Address", pos=(10, 50), size=(120, 25))
        self.userNameLabel = wx.StaticText(self, label="UserName", pos=(40, 100), size=(120, 25))
        self.serverAddress = wx.TextCtrl(self, pos=(120, 47), size=(150, 25),style=wx.TE_PROCESS_TAB)
        self.userName = wx.TextCtrl(self, pos=(120, 97), size=(150, 25),style=wx.TE_PROCESS_TAB)
        self.loginButton = wx.Button(self, label='Login', pos=(80, 145), size=(130, 30))
        # 绑定登录方法
        self.loginButton.Bind(wx.EVT_BUTTON, self.login)
        self.Show()#显示以上控件
    
    def login(self, event): 
        pass
      
if __name__ == '__main__':
    app = wx.App()#实例化一个主循环
    LoginFrame(None, -1, title="Login", size=(320, 250))
    app.MainLoop()#启动主循环
