import sys
import matplotlib.pyplot as plt
import time
import random

import pysftp


def getData(file):
    while True:
        line = file.readline()
        if line:
            return line


def show_finger_pos(dataFiles):
    plt.axis()
    plt.ion()
    while True:
        line = list(map(lambda x: getData(x), dataFiles))
        pos_str = line[0].split(',')
        x = float(pos_str[0])
        y = float(pos_str[1])
        plt.scatter(x, y, 'r')
        plt.pause(0.01)


def linegraph(dataFiles):
    plt.axis()
    plt.ion()
    start = int(round(time.time() * 1000))
    map(lambda x: x.seek(0), dataFiles)
    weights = [[0.0], [0.0], [0.0], [0.0]]
    xs = [0]
    while True:
        lines = list(map(lambda x: float(getData(x)), dataFiles))
        print(lines)
        if lines:
            xs.append(int(round(time.time() * 1000)) - start)
            for i, weight in enumerate(lines):
                weights[i].append(weight)

            plt.plot(xs, weights[0], 'r', xs, weights[1], 'g', xs, weights[2], 'b', xs, weights[3], 'y')
            plt.pause(0.01)


if __name__ == '__main__':
    if len(sys.argv) < 3 or sys.argv[2] == "help":
        print("log_weights [weights|pos] [files]+ [host]? [username]? [password]?")
    filenames = sys.argv[2:6]
    print(filenames)
    if len(sys.argv) > 6:
        host = sys.argv[5]
        username = sys.argv[6]
        password = sys.argv[7]
        srv = pysftp.Connection(host=host, username=username, password=password)
        dataFiles = list(map(lambda filename: srv.open(filename, "r"), filenames))
    else:
        dataFiles = list(map(lambda filename: open(filename, "r"), filenames))

    if sys.argv[1] == "pos":
        show_finger_pos(dataFiles)
    else:
        linegraph(dataFiles)
