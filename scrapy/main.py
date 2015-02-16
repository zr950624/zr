import scrapy
import re
from wallpaper.items import WallpaperItem
from scrapy.selector import Selector
from scrapy.http import Request
import urllib2
import socket
#response.xpath("//a[@target='_blank'][@data-click]/@href").extract()
#response.xpath("//div[@id='page']/a[10]/@href").extract()

class BaiduSpider(scrapy.Spider):
    name = "baidu"
    start_urls = ["http://www.baidu.com/s?wd=headless%E7%88%AC%E8%99%AB&rsv_spt=1&issp=1&f=8&rsv_bp=0&rsv_idx=2&ie=utf-8&tn=baiduhome_pg&rsv_enter=1&rsv_sug3=9&rsv_sug4=1371&rsv_sug1=8&rsv_pq=f2dc9a9c00006696&rsv_t=ecc9sHSMjnEI3yacueEV5lmN%2B%2F8FpibMtmDqDux7957MHUKlJNGvULp3b1YrREKF2gZv&rsv_sug2=0&inputT=5440"]

    time = 0
    def parse(self,response):
        if self.time > 2:        
            print"---------------------------\n",next_page,'-------------------------\n'
            pass
        self.time = self.time + 1
        sel = Selector(response)
        next_page = sel.xpath("//div[@id='page']/a[last()]/@href").extract()
        next_page = "http://www.baidu.com"+next_page[0]

        items = []
        content = sel.xpath("//a[@target='_blank'][@data-click]/@href").extract()
        f = open('link.txt','ab+')
        for url in content:
            try:
                req = urllib2.Request(url)
                res = urllib2.urlopen(req,timeout = 20)
            except urllib2.HTTPError, e:
                f.write(url+"        "+str(e.code)+e.reason+'\n')
            except urllib2.URLError, e:
                f.write(url+"        cannot reach"+e.reason+'\n')
            except socket.timeout, e:
                f.write(url+"        timeout\n")
            else:
                newurl = res.geturl()
                f.write(newurl+"      200 OK\n")
        f.close()
        print "----------------------\n",self.time,"------------------\n"
        
        return Request(next_page)

