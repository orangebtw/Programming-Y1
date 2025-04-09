import random

with open("./data_big.txt", "w") as f:
    for i in range(10 ** 6):
        f.write(str(random.uniform(float(-(10 ** 3)), float(10 ** 3))))
        f.write('\n')