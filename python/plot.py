from matplotlib import pyplot as plt

with open('../out.txt','r') as f:
  data=[float(line[:-1]) for line in f.readlines()]
  plt.plot(data)

  plt.savefig('plot.pdf')
