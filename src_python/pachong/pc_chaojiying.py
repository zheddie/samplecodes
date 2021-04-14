from selenium.webdriver import Chrome
import time
from selenium.webdriver.common.keys import Keys
from chaojiying import Chaojiying_Client
web = Chrome()


web.get("http://www.chaojiying.com/user/login/")
time.sleep(3)
img = web.find_element_by_xpath('/html/body/div[3]/div/div[3]/div[1]/form/div/img').screenshot_as_png

chaojiying = Chaojiying_Client('zheddie', 'Gutfee1ing', '915145')	#用户中心>>软件ID 生成一个替换 96001												#本地图片文件路径 来替换 a.jpg 有时WIN系统须要//
dic = chaojiying.PostPic(img, 1902)
verify_code = dic['pic_str']

web.find_element_by_xpath('/html/body/div[3]/div/div[3]/div[1]/form/p[1]/input').send_keys("zheddie")
web.find_element_by_xpath('/html/body/div[3]/div/div[3]/div[1]/form/p[2]/input').send_keys("Gutfee1ing")
web.find_element_by_xpath('/html/body/div[3]/div/div[3]/div[1]/form/p[3]/input').send_keys(verify_code)
time.sleep(5)
web.find_element_by_xpath('/html/body/div[3]/div/div[3]/div[1]/form/p[4]/input').click()
