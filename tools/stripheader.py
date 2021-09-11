# removes first 10
import sys 
f = open(sys.argv[1], 'rb')
by = f.read()
f.close()
i = 10
f = open(sys.argv[1], 'wb')
while i < len(by):
    f.write(bytes([by[i]]))
    i += 1
f.close()
