import numpy as np
from matplotlib import pyplot as plt

a=2048
b=2**20//a#*8-7
with open('../out.txt','r') as f:
  x, y = np.mgrid[:a, :b]
  z=np.zeros((a,b))
  for i in range(0,b):
    for j in range(0,a):
      z[j][i]=float(f.readline())
  fig, ax = plt.subplots()
  im = ax.pcolormesh(y, x, z)
  fig.colorbar(im, ax=ax)

  plt.savefig('plot.png', dpi=600)

