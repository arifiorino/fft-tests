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

from scipy.io import wavfile
samplerate, data = wavfile.read('bird.wav')
#print(samplerate)
length = 8192
print(length)
avg = sum(data)/len(data)
r = max(min(data), max(data))
for x in data[:length]:
  print((x-avg)/r)
