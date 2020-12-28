'''
import math

rate = 44100
fund = 440

length = 2

with open("test.txt","w") as f:
  f.write(str(length * rate)+"\n")
  for i in range(length * rate):
    f.write(str(math.sin(2*math.pi*i/fund))+"\n")
'''

'''
from scipy.io import wavfile
samplerate, data = wavfile.read('bird.wav')
#print(samplerate)
length = 8192
print(length)
avg = sum(data)/len(data)
r = max(min(data), max(data))
for x in data[:length]:
  print((x-avg)/r)
'''

import cmath, math, random
length=2**18
freq=440
rate=44100
D=[]
with open("test.txt","w") as f:
  f.write(str(length)+"\n")
  for i in range(length//2-1):
    if i!=0 and i%int(freq*length/rate)==0:
      x=cmath.rect(100000,random.uniform(0,2*math.pi))
      D.append(x)
    else:
      D.append(0)
  f.write(str(0)+"\n")
  f.write(str(0)+"\n")
  for i in range(length//2-1):
      f.write(str(D[i].real)+"\n")
      f.write(str(D[i].imag)+"\n")
  f.write(str(0)+"\n")
  f.write(str(0)+"\n")
  for i in range(length//2-1):
      f.write(str(D[len(D)-i-1].real)+"\n")
      f.write(str(-D[len(D)-i-1].imag)+"\n")
