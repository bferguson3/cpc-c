#!/usr/bin/python3
import os,sys
f = open(sys.argv[1], "rb")
inby = f.read()
f.close()
fsize = len(inby)
#print(inby)

hby=[]
i = 0
while i < 0x100:
	hby.append(0)
	i += 1
# header record:
hby[0x1] = ord('M')
hby[0x2] = ord('A')
hby[0x3] = ord('I')
hby[0x4] = ord('N')
hby[0x5] = ord(' ')
hby[0x6] = ord(' ')
hby[0x7] = ord(' ')
hby[0x8] = ord(' ')
hby[0x9] = ord('B')
hby[0xa] = ord('I')
hby[0xb] = ord('N')
hby[0xc] = 0
hby[0xd] = 0
hby[0xe] = 0
hby[0xf] = 0
hby[0x10] = 0
hby[0x11] = 0
hby[18] = 2 # bin
hby[19] = fsize & 0xff
hby[20] = fsize >> 8  # data length
hby[21] = 0x38
hby[22] = 0x1  #data location
hby[23] = 0xff
hby[24] = fsize & 0xff
hby[25] = fsize >> 8  #logical length
hby[26] = 0x38 
hby[27] = 0x1   #entry address
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


f = open('MAIN.BIN','wb')
i = 0
while i < 128:
	f.write(hby[i])
	i += 1
i = 0
while i < len(inby):
	f.write(bytes([inby[i]]))
	i += 1
