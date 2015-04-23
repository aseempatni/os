#!/usr/bin/env python
import numpy as np
import matplotlib.pyplot as plt
import sys
from matplotlib.legend_handler import HandlerLine2D

# read data
data = []
input_file_name = sys.argv[1]
with open(input_file_name,"r") as f:
	f.readline()
	f.readline()
	while 1:
		line = f.readline()
		if not line:
			break
		data.append(line.split())

data = np.array(data)

# plot data
x = data[:,0]
fifo = data[:,1]
lru = data[:,2]
lfu = data[:,3]
second_chance = data[:,4]
plt.figure()
a, = plt.plot(x, fifo,  linestyle='--', color='r', label='FIFO')
b, = plt.plot(x, lru,  linestyle='--', color='g', label='LRU')
c, = plt.plot(x, lfu,  linestyle='--', color='b', label="LFU")
d, = plt.plot(x, second_chance, linestyle='--', color='c', label="SC")
plt.title("Page Replacement")
plt.xlabel("No of frames")
plt.ylabel("Page Faults")
plt.legend(handler_map={a: HandlerLine2D(numpoints=4)})

# save and show plot
plt.savefig(input_file_name)
plt.show()
