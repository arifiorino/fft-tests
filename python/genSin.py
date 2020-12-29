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
import numpy as np
length=2**18
freq=440
rate=44100
D=np.zeros(length,dtype=np.complex_)
with open("test.txt","w") as f:
  f.write(str(length)+"\n")
  freq2=int(freq*length/rate)
  for i in range(1,(length//2)//freq2):
    D[i*freq2]=cmath.rect(100000/(2**(i-1)),random.uniform(0,2*math.pi))
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
