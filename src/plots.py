#!/usr/bin/env python3
from pandas import read_table, DataFrame
import pandas as pd
import numpy as np
import matplotlib.pyplot as plt


# def get_data(filename):
#     """
#     extract data X and Y from the file txt
#     """
#     data = importdata(filename)
#     return data



### Constantes ###

# plots useful to grasp influence of the sub order of correlation_rate 
fp = "../build/data_to_python.txt" # column = {'coordinates X', 'coordinates Y'}
fp2 = "../data/results/loss_function_translation.txt"        # translation data
fp21 = "../data/results/loss_function_translation1.txt"        # translation data
fp22 = "../data/results/loss_function_translation2.txt"        # translation data
fp23 = "../data/results/loss_function_translation3.txt"        # translation data
fp25 = "../data/results/loss_function_translation5.txt"
fp28 = "../data/results/loss_function_translation8.txt"

fp3 = "../data/results/corr_function_rotation.txt"             # rotation data
fp32 = "../data/results/corr_function_rotation2.txt"           # rotation data
fp36 = "../data/results/corr_function_rotation6.txt"           # rotation data
fp310 = "../data/results/corr_function_rotation10.txt"         # rotation data


fp_exec = "../data/results/loss_function_translation_tx.txt"



def main(filename):

    data = read_table(filename, sep = ",")
    if (filename == fp2 or filename == fp21 
    or filename == fp22 or filename == fp23 
    or filename == fp25 or filename == fp28
    or filename == fp_exec):
        data.plot(x = "p_x", y = "loss", kind = 'line')
    
    if (filename == fp3 or filename == fp32 
    or filename == fp36 or filename == fp310) :
        data.plot(x = 'theta', y = 'correlation rate') 

    plt.show()
    return



def histogram():

    # fig = plt.figure()
    # ax1 = fig.add_subplot(111)

    fig, ax1 = plt.subplots()
    data = read_table("../data/results/histogram.txt", sep=",")
    data2 = read_table("../data/results/interclassvariance.txt", sep=",")
    ax1.plot(data["x"], data["y"], color="c")
    ax1.set_ylabel("Pixel Count", color="c")
    ax1.set_xlabel('Pixel Intensity')
    ax1.fill_between(data["x"], 0, data["y"], color="c")
    ax2 = ax1.twinx()
    ax2.plot(data2["x"],data2["y"], color="orangered")
    
    ax2.set_ylabel("Inter-Class Variance", color="orangered")
    # plt.plot()
    # plt.add_subplot(111)
    # data.plot(x = "x", y = "y", kind = 'line')
    # data2.plot(x = "x", y = "y", kind = 'line', color="red")
    fig.tight_layout()  # otherwise the right y-label is slightly clipped
    plt.savefig('../data/results/plotOTSU.png')
    plt.show()
    return


def variance():
    data = read_table("../data/results/interclassvariance.txt", sep = ",")
    data.plot(x = "x", y = "y", kind = 'line')
    plt.show()
    return


# main(fp2)
# main(fp21)
# main(fp22)
# main(fp23)
# main(fp25)
# main(fp28)

# main(fp3)
# main(fp32)
# main(fp36)
# main(fp310)

# histogram()
# variance()

main(fp_exec)


