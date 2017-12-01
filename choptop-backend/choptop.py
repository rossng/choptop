from flask import Flask
from sensor import Sensor
import RPi.GPIO as GPIO
import time
import sched
from plotter import Plotter
import numpy as np
import asyncio
import websockets
app = Flask(__name__)


class ChopTop:
    def __init__(self):
        self.scheduler = sched.scheduler(time.time, time.sleep)
        self.period = 1.0 / 60
        timestr = time.strftime("%Y%m%d-%H%M%S") + '.log'
        self.log_file = open("logs/" + timestr, "w")
        self.sensors = []
        self.sensors.append(Sensor(5, 6, -0.3737, 0))
        self.sensors.append(Sensor(7, 8, -0.5563, -1))  # placeholder pins
        self.sensors.append(Sensor(9, 10, 1, 0))
        #self.sensors.append(Sensor(20, 21, 1, 0))
        self.finger_position = (0, 0)
        #self.plotter = Plotter()
        self.weights = np.array([0,0,0])

    async def hello(uri):
        async with websockets.connect(uri) as websocket:
            await websocket.send("Hello world!")


    def start(self):

        asyncio.get_event_loop().run_until_complete(
                hello('ws://localhost:8765'))

        for sensor in self.sensors:
            sensor.start()

        self.scheduler.enter(0, 1, self.update, ())
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
            x = clamp(float(self.weights[0] + self.weights[1]) / np.sum(self.weights), 0, 1)
            y = clamp(float(self.weights[1] + self.weights[2]) / np.sum(self.weights), 0, 1)
            self.finger_position = (x, y)
            #self.plotter.queue.put(self.finger_position)
        print(str(self.finger_position))
        #print str(self.weights)
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
