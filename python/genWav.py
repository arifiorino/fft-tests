from scipy.io import wavfile
import numpy as np

with open('out.txt','r') as f:
  data=[float(line[:-1]) for line in f.readlines()]
  data=np.array(data)
  r=max(max(data),-min(data))
  data /= r
  wavfile.write("python/synth.wav", 44100, data)
