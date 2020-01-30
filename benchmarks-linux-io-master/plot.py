#!/usr/bin/python3

import matplotlib.pyplot as plt
#from math import *
#from time import sleep
import csv
import numpy as np
import sys

import pandas as pd


xlist = []
ylist = []

def parse_csv(filename):

    with open(filename, 'r') as file:
        datareader = csv.reader(file, delimiter=',')
        for row in datareader:
            print(', '.join(row))
            xlist.append(float(row[0]))
            ylist.append(float(row[4]))

    
    return

def _main():    

    if (len(sys.argv) < 1):
        print("Not enough arguments")
        exit(-1)
        
    #parse_csv(sys.argv[1])
    data = pd.read_csv(sys.argv[1])
    spacing = data.groupby(['spacing'])['spacing'].mean()
    duration = data.groupby(['spacing'])['req'].mean()
    duration_std = data.groupby(['spacing'])['req'].std()
                            
    print(spacing)
    print(duration)
    print(duration_std)
    
    fig = plt.figure()
    ax = plt.subplot(111)
    
    ax.margins(y=0)
    
    fig.suptitle("Courbe avec intervalle de confiance de 95% pour écritures de taille variable séparées \npar trous d'une page")
    plt.xlabel("Longeur des ecritures en pages")
    plt.ylabel("Nombre de requetes effectuee")
    
    #order = np.argsort(xlist)
    #xs = np.array(xlist)[order]
    #ys = np.array(ylist)[order]
    
    plt.errorbar(spacing, duration, yerr=2*duration_std, ecolor='black')
    ax.legend("Nombres écritures = 67108864 octets (64MB) = 16384 pages, taille chaque écriture = 4096 octets = 1 page")
    
    ax.axhline(y=0, color='k')
    
    plt.show()

    return  


_main()


    



