from selenium.webdriver import Chrome
import time
from selenium.webdriver.common.keys import Keys
web = Chrome()

#web.get("http://www.baidu.com")

web.get("https://lagou.com")

#查找“全国”的按钮的xpath
el = web.find_element_by_xpath('//*[@id="changeCityBox"]/ul/li[1]/a')
#print(web.title)
el.click()
#查找输入框
time.sleep(2)
web.find_element_by_xpath('//*[@id="search_input"]').send_keys("python",Keys.ENTER)

li_list = web.find_elements_by_xpath ('//*[@id="s_position_list"]/ul/li')
for li in li_list:
    jobname = li.find_element_by_tag_name("h3").text
    jobprice = li.find_element_by_xpath('./div/div/div[2]/div/span').text
    compname = li.find_element_by_xpath('./div/div[2]/div/a').text
    print(jobname,compname,jobprice)