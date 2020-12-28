from matplotlib import pyplot as plt

with open('../out.txt','r') as f:
  data=[]
  data.append(float(f.readline()))
  plt.plot(data)

  plt.savefig('plot.png')
  plt.savefig('plot.pdf')

