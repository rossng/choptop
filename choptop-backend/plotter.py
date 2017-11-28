import matplotlib.pyplot as plt
from multiprocessing import Process, Queue

import time


class Plotter:
    def __init__(self):

        self.queue = Queue(10)
        self.process = Process(target=self.plot_data)
        self.process.start()

    def plot_data(self):
        self.fig=plt.gcf()
        self.fig.show()
        plt.axis([0, 1, 0, 1])
        plt.ion()
        while True:
            data = self.queue.get()
            plt.scatter(data[0], data[1])
            self.fig.canvas.draw()
            time.sleep(1/60)