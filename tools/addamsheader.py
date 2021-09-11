#!/usr/bin/python3
import os,sys
f = open(sys.argv[1], "rb")
inby = f.read()
f.close()
fsize = len(inby)
#print(inby)
fn = sys.argv[1].split('.')
#print(fn[0][0:8],fn[1][0:3])
fex = fn[1][0:3]
fn = fn[0][0:8]
while(len(fn) < 8):
	fn = fn + ' '
loadloc = 0x8000
startloc = 0x8000

if(len(sys.argv) < 4):
	print('usage: $ python3 addamsheader.py <file> <loadloc> <startloc>\n')
	sys.exit()

if(sys.argv[2][1].lower() == 'x'):
	loadloc = int(sys.argv[2],16)
if(sys.argv[3][1].lower() == 'x'):
	startloc = int(sys.argv[3],16)

fn = fn.upper()
fex = fex.upper()	

hby=[]
i = 0
while i < 0x100:
	hby.append(0)
	i += 1
# header record:
hby[0x1] = ord(fn[0]) 
hby[0x2] = ord(fn[1])
hby[0x3] = ord(fn[2])
hby[0x4] = ord(fn[3])
hby[0x5] = ord(fn[4])
hby[0x6] = ord(fn[5])
hby[0x7] = ord(fn[6])
hby[0x8] = ord(fn[7])
hby[0x9] = ord(fex[0])
hby[0xa] = ord(fex[1])
hby[0xb] = ord(fex[2])
hby[0xc] = 0
hby[0xd] = 0
hby[0xe] = 0
hby[0xf] = 0
hby[0x10] = 0
hby[0x11] = 0
hby[18] = 2 # bin
hby[19] = (fsize & 0xff)
hby[20] = (fsize >> 8)  # data length
#print(hby[19])
#print(hby[20])
hby[21] = loadloc & 0xff
hby[22] = loadloc >> 8  #data location
hby[23] = 0xff
hby[24] = fsize & 0xff
hby[25] = fsize >> 8  #logical length
hby[26] = startloc & 0xff
hby[27] = startloc >> 8   #entry address
# 28..63 = 0
i = 28
while i < 64:
	hby[i] = 0
	i += 1
# disk header:
hby[64] = fsize & 0xff
hby[65] = fsize >> 8
hby[66] = 0
chk = 0
i = 0
while i < 67:
	chk += hby[i]
	i += 1
hby[67] = chk & 0xff
hby[68] = chk >> 8

i = 0
while i < 128:
	hby[i] = bytes([hby[i]])
	i += 1


f = open(fn+'.'+fex,'wb')
i = 0
while i < 128:
	f.write(hby[i])
	i += 1
i = 0
while i < len(inby):
	f.write(bytes([inby[i]]))
	i += 1
