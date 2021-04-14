from selenium.webdriver import Chrome
import time
from selenium.webdriver.common.keys import Keys
from selenium.webdriver.chrome.options import Options
opt = Options()
opt.add_argument("--disable-blink-features=AutomationControlled")
web = Chrome(options=opt)

web.get("https://weibo.com/")
time.sleep(8)
user = web.find_element_by_xpath('//*[@id="loginname"]')
user.send_keys("18910572062")
passwd = web.find_element_by_xpath('//*[@id="pl_login_form"]/div/div[3]/div[2]/div/input')
passwd.send_keys("Gutfee1ing")
denglu = web.find_element_by_xpath('//*[@id="pl_login_form"]/div/div[3]/div[6]/a/span')
denglu.click()