import sys

f = open(sys.argv[1], "r")

inside_iteration = False
iteration_num = 0
fitness_list = []
std_dev_list = []
d0_list = []
d1_list = []
dun_list = []
att_list = []

go_for_fitness = False
go_for_std_dev = False

for line in f:
    if inside_iteration:
        if line.startswith("Gen"):
            go_for_fitness = True
        elif line.startswith("FINAL"):
            inside_iteration = False
        elif go_for_fitness:
            fitness = float(line.split(',')[-1])
            fitness_list[-1].append(fitness)
            d0_list[-1].append(float(line.split(',')[0]))
            d1_list[-1].append(float(line.split(',')[1]))
            dun_list[-1].append(float(line.split(',')[2]))
            att_list[-1].append(float(line.split(',')[-2]))
            go_for_fitness = False
            go_for_std_dev = True
        elif go_for_std_dev:
            std_dev_list[-1].append(float(line))
            go_for_std_dev = False
            
    if line.startswith("Iteration"):
        inside_iteration = True
        iteration_num += 1
        print(iteration_num)
        fitness_list.append([])
        std_dev_list.append([])
        d0_list.append([])
        d1_list.append([])
        dun_list.append([])
        att_list.append([])


def normalize(l):
    max_val = max(l)
    min_val = min(l)
    return [(x - min_val) / (max_val - min_val) for x in l] 

import numpy
def movingaverage(interval, window_size):
    window= numpy.ones(int(window_size))/float(window_size)
    return numpy.convolve(interval, window, 'same')

import matplotlib.pyplot as plt
'''
for i in range(min(5, len(fitness_list))):
    plt.plot(range(len(fitness_list[i])),normalize(fitness_list[i]))
    plt.plot(range(len(std_dev_list[i])),normalize(std_dev_list[i]))
    plt.legend(['fitness', 'std deviation of fitness'], loc='upper right')
    plt.show()
'''

for i in range(min(5, len(fitness_list))):
    plt.plot(range(len(fitness_list[i])),fitness_list[i])
axes = plt.gca()
#axes.set_ylim([-2.23e7, -2.13e7]) 
plt.show()

for i in range(min(5, len(fitness_list))):
    plt.plot(range(len(std_dev_list[i])),std_dev_list[i])

plt.show()
'''
for i in range(min(5, len(fitness_list))):
    plt.plot(range(len(std_dev_list[i])),movingaverage(std_dev_list[i], 20))

plt.show()
'''
for i in range(min(5, len(fitness_list))):
    plt.plot(range(len(d0_list[i])),d0_list[i])

plt.show()

for i in range(min(5, len(fitness_list))):
    plt.plot(range(len(d1_list[i])),d1_list[i])

plt.show()

for i in range(min(5, len(fitness_list))):
    plt.plot(range(len(dun_list[i])),dun_list[i])

plt.show()

for i in range(min(5, len(fitness_list))):
    plt.plot(range(len(att_list[i])),att_list[i])

plt.show()


for i in range(min(5, len(fitness_list))):
    print(len(fitness_list[i]))
    print(len(std_dev_list[i]))
