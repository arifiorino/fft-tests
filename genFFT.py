import random
m = 8
with open("test.txt", "w") as f:
  
  f.write(str(m)+"\n")
  for i in range(m):
    f.write(str(random.uniform(0,100))+"\n")
