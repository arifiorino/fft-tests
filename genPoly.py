import random
m = 2 ** 20 # 2 ** 23
with open("test.txt", "w") as f:
  
  f.write(str(m)+"\n")
  for i in range(2 * m):
    f.write(str(random.randint(0,100))+"\n")
