#!/usr/bin/env python
import numpy as np
import matplotlib.pyplot as plt
import sys

# raw_input()
# raw_input()
# print nframe, fifo, lru, lfu, second_chance

afifo = []
alru = []
alfu = []
asecond = []
anframe = []

data = []
input_file_name = sys.argv[1]
print input_file_name
with open(input_file_name,"r") as f:
	f.readline()
	f.readline()
	while 1:
	# for i in range(1,f.size()):
		line = f.readline()
		if not line:
			break
		# print line.split()
		nframe, fifo, lru, lfu, second_chance = line.split();
		data.append(line.split())
		# afifo.append(fifo)
		# alru.append(lru)
		# alfu.append(lfu)
		# asecond.append(second_chance)
		# anframe.append(nframe)
data = np.array(data)
print data

x = data[:,0]
fifo = data[:,1]
lru = data[:,2]
lfu = data[:,3]
second_chance = data[:,4]
# handles, labels = ax.get_legend_handles_labels()
# ax.legend(handles, labels)
# # pname = ["p = 0.1","p = 0.5","p = 0.9","p = 1.0"]
pname = ["fifo","lru","lfu","sc"]
plt.figure()
a = plt.plot(x, fifo,  linestyle='--', color='r', label='fifo')
b = plt.plot(x, lru,  linestyle='--', color='g')
c = plt.plot(x, lfu,  linestyle='--', color='b')
d = plt.plot(x, second_chance, linestyle='--', color='c')
# plt.legend(handles=[a,b,c,d])
plt.savefig("Throughput")
plt.show()
# plt.legend(aplot,pname)


# aplot = []
# aplot.append (plt.errorbar(x, atm[j], atd[j])[0])
# aplot.append (plt.errorbar(x, atm[j], atd[j])[0])
# aplot.append (plt.errorbar(x, atm[j], atd[j])[0])
# aplot.append (plt.errorbar(x, atm[j], atd[j])[0])
# # for j in range(4):
# plt.title("Throughput Performance")
# plt.xlabel("Data Generation Rate(Kbps)")
# plt.ylabel("Throughput")
# plt.show()


'''
prefix = "backoff_"

d1 = sys.argv[1]
d2 = sys.argv[2]
nplot = len(sys.argv) - 1
pname = sys.argv[1:]

atm = []
atd = []
adm = []
add = []
ajm = []
ajd = []

for j in range(nplot):
	tm = []
	td = []
	dm = []
	dd = []
	jm = []
	jd = []
	with open(pname[j],"r") as f:
		for i in range(1,8):
			f.readline()
			mean = f.readline().split()
			tm.append (float(mean[0]))
			dm.append (float(mean[1]))
			jm.append (float(mean[2]))
			dev = f.readline().split()
			td.append (float(dev[0]))
			dd.append (float(dev[1]))
			jd.append (float(dev[2]))
	atm.append(tm)
	atd.append(td)
	adm.append(dm)
	add.append(dd)
	ajm.append(jm)
	ajd.append(jd)

x = np.array([16,32,64,128,256,512,1024])
y = dm

# pname = ["p = 0.1","p = 0.5","p = 0.9","p = 1.0"]
pname = ["exponential","linear"]
# Plot throughput
plt.figure()
aplot = []
for j in range(nplot):
	aplot.append (plt.errorbar(x, atm[j], atd[j])[0])
plt.legend(aplot,pname)
plt.title("Throughput Performance")
plt.xlabel("Data Generation Rate(Kbps)")
plt.ylabel("Throughput")
plt.savefig(prefix+"Throughput")
plt.show()

# Plot forward delay
plt.figure()
aplot = []
for j in range(nplot):
	aplot.append (plt.errorbar(x, adm[j], add[j])[0])
plt.legend(aplot,pname)
plt.title("Forward Delay Performance")
plt.xlabel("Data Generation Rate(Kbps)")
plt.ylabel("Forwarding Delay")
plt.savefig(prefix+"ForwardDelay")
plt.show()

# Plot jitter
plt.figure()
aplot = []
for j in range(nplot):
	aplot.append (plt.errorbar(x, ajm[j], ajd[j])[0])
plt.legend(aplot,pname)
plt.title("Jitter Performance")
plt.xlabel("Data Generation Rate(Kbps)")
plt.ylabel("Jitter")
plt.savefig(prefix+"Jitter")
plt.show()
'''