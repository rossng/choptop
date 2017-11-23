from flask import Flask
from sensor import Sensor
import RPi.GPIO as GPIO
import time
import sched
import matplotlib
#matplotlib.use('GTK')
import matplotlib.pyplot as plt
import numpy as np
app = Flask(__name__)


class ChopTop:
    def __init__(self):
        self.scheduler = sched.scheduler(time.time, time.sleep)
        self.period = 1.0 / 10
        timestr = time.strftime("%Y%m%d-%H%M%S") + '.log'
        self.log_file = open("logs/" + timestr, "w")
        self.sensors = []
        self.sensors.append(Sensor(5, 6))
        self.sensors.append(Sensor(7, 8))  # placeholder pins
        self.sensors.append(Sensor(9, 10))
        self.sensors.append(Sensor(20, 21))
        self.finger_position = (0, 0)
        #self.fig=plt.gcf()
        #self.fig.show()
        self.weights = np.array([0,0,0,0])
        #plt.axis([0, 1, 0, 1])
        #plt.ion()

    def start(self):
        self.scheduler.enter(self.period, 1, self.update, ())
        try:
            self.scheduler.run()
        except KeyboardInterrupt:
            GPIO.cleanup()
            self.log_file.close()

    def update(self):
        logtext = ""
        # get weights for each sensor
        for i, sensor in enumerate(self.sensors):
            weight = sensor.get_weight()
            self.weights[i] = weight
            #logtext += str(weight) + ","

        if np.sum(self.weights):
            x = clamp(float(self.weights[1] + self.weights[2]) / np.sum(self.weights), 0, 1)
            y = clamp(float(self.weights[2] + self.weights[3]) / np.sum(self.weights), 0, 1)
            self.finger_position = (x, y)
            #plt.scatter(x, y)
            #self.fig.canvas.draw()
        print str(self.finger_position)
        #self.log_file.write(logtext + str(int(round(time.time() * 1000))) + '\n')
        self.scheduler.enter(self.period, 1, self.update, ())


def clamp(n, minn, maxn):
    return max(min(maxn, n), minn)


def main():
    # my code here
    choptop = ChopTop()
    # app = Flask(__name__)
    # app.config['ChopTop'] = choptop
    choptop.start()


if __name__ == "__main__":
    main()
