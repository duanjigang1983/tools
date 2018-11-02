#!/usr/bin/env python
# -*- coding: utf-8 -*- 
""" 
__author__= 'Du' 
__creation_time__= '2018/1/5 9:08' 
"""
  
import os, math 
import sys
from PIL import Image, ImageFont, ImageDraw
  
  
def circle(): 
	ima = Image.open("4.png").convert("RGBA") 
 	# ima = ima.resize((600, 600), Image.ANTIALIAS) 
	size = ima.size 
	print(size) 
  
 # 因为是要圆形，所以需要正方形的图片 
	r2 = min(size[0], size[1]) 
	if size[0] != size[1]: 
		ima = ima.resize((r2, r2), Image.ANTIALIAS) 
  
 # 最后生成圆的半径 
	r3 = 200
	imb = Image.new('RGBA', (r3*2, r3*2),(255,255,255,0)) 
	pima = ima.load() # 像素的访问对象 
	pimb = imb.load() 
	r = float(r2/2) #圆心横坐标 
  
	for i in range(r2): 
		for j in range(r2): 
			lx = abs(i-r) #到圆心距离的横坐标 
			ly = abs(j-r)#到圆心距离的纵坐标 
			l = (pow(lx,2) + pow(ly,2))** 0.5 # 三角函数 半径 
  			if l < r3: 
				pimb[i-(r-r3),j-(r-r3)] = pima[i,j] 
	imb.paste(imb, (0, 0, 400, 400), imb)	
	imb.save("circle.png") 

if __name__ == '__main__':
	
	circle()
	output = "output-%s.png" % os.getpid()
	if(len(sys.argv) >= 1):
		output = "%s.png" % sys.argv[1]
	print output
	im = Image.open("circle.png")
	#im  = Image.new('RGBA', im.size, (100,100,150))
	#im.paste(im, (0, 0, 400, 400), im)
	#im.paste(im, (50,50,200))

	ttfont = ImageFont.truetype("/usr/share/fonts/msyh.ttf", 60) 
	draw = ImageDraw.Draw(im)
	name = sys.argv[1][0:10]
	pos = 120 - 10*len(name)/2 

	pos = pos - int(len(name)/10) - 20*(int(len(name)/4))

	draw.text((pos,150), name, fill=(256,256,0), font=ttfont)


	ttfont = ImageFont.truetype("/usr/share/fonts/msyh.ttf", 20) 
	draw = ImageDraw.Draw(im)
	draw.text((180,350), u'OPSX', fill=(200,200,200), font=ttfont)

	im.save(output)
