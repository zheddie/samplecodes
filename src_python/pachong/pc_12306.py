from selenium.webdriver import Chrome
import time
from selenium.webdriver.common.keys import Keys
from chaojiying import Chaojiying_Client
from selenium.webdriver.common.action_chains import ActionChains
from selenium.webdriver.chrome.options import Options

chaojiying = Chaojiying_Client('zheddie', 'Gutfee1ing', '915145')	#用户中心>>软件ID 生成一个替换 96001												#本地图片文件路径 来替换 a.jpg 有时WIN系统须要//
#如何伪装当前Chrome为真正的Chrome。
# if chrome 版本<88
#window.nevigator.webdriver
# web= Chrome()
# web.execute_cdp_cmd("Page.addScriptToEvaluateOnNewDocument",{
#   "source": """
#   window.navigator.webdriver = undefined
#       Object.defineProperty(navigator,'webdriver', {
#           get:() => undefined
#       })
#   """
#})
#web.get(xxxx)
# 
# 如果chrome>88
# option = Options()
# option.add_argument("--disable-blink-features=AutomationControlled")

option = Options()
option.add_argument('--disable-blink-features=AutomationControlled')

web = Chrome(options=option)

web.get("https://kyfw.12306.cn/otn/resources/login.html")
time.sleep(3)
web.find_element_by_xpath('/html/body/div[2]/div[2]/ul/li[2]/a').click()
time.sleep(5)
verify_img_el = web.find_element_by_xpath('//*[@id="J-loginImg"]')
img = verify_img_el.screenshot_as_png
dic = chaojiying.PostPic(img, 9004)
result = dic['pic_str']
rs_list = result.split('|')
for rs in rs_list:
    points = rs.split(',')
    x = int(points[0])
    y = int(points[1])
    ActionChains(web).move_to_element_with_offset(verify_img_el,x,y).click().perform()

user = web.find_element_by_xpath('//*[@id="J-userName"]')
passwd = web.find_element_by_xpath('//*[@id="J-password"]')
user.send_keys("zheddie")
passwd.send_keys("Gutfee2ing")
denglu = web.find_element_by_xpath('//*[@id="J-login"]')
denglu.click()
time.sleep(5)
#拖拽
btn = web.find_element_by_xpath('//*[@id="nc_1_n1z"]')
                                
ActionChains(web).drag_and_drop_by_offset(btn,300,0).perform()

