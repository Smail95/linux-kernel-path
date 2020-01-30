#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Tue Mar 07 20:58 2019

@author: aider
"""

import json
from matplotlib import pyplot as plt
from matplotlib import style
import numpy as np
import sys

# parse input json file
def read_json(path, disk_dev, torecord):
    dic =  {}
    x = []
    y = []
    shift = 0
    sums = 0
    # open input file 
    json_file = open(path)
    data = json.load(json_file)
   
    # record 'torecord' data of 'disk-dev' type  
    data = data['sysstat']['hosts']
    data = data[len(data)-1]
    for stat in data['statistics']:
        if len(stat) == 0: # ignoring the last field 
            break
        disk = stat['disk']
        for line in disk:
            if line['disk_device'] == disk_dev:
                x += [shift,]
                y += [line[torecord],]
                shift += 1
                sums += line[torecord]
    dic[0] = x
    dic[1] = y 
    print(sums/x[-1])
    print('\n')
    print(x[-1])
    
    return dic



# plot data
def plot(data):
    style.use('ggplot')
    x = data[0]
    y = data[1]

    fig, ax = plt.subplots()
    plt.plot(x, y)

    #plt.title('The average queue length of the requests \nthat were issued to the device.')
    #plt.ylabel('number of write request')
    plt.ylabel('queue length')
    plt.xlabel('time (sec)')
    
    fig.savefig('2go_1page_offset.png')


functions = {
    'plot': plot,
}

if __name__ == '__main__':
	func = functions[sys.argv[1]]
	args = sys.argv[2:]
	data = read_json(*args)
	func(data)
	sys.exit()	


	
