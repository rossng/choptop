import threading
from collections import deque
from hx711 import HX711
from multiprocessing import Process, Queue, Pool
import RPi.GPIO as GPIO


class Sensor:
    def __init__(self, dout, pd_sck):
        load_sensor = HX711(dout, pd_sck)
        load_sensor.set_reading_format("LSB", "MSB")
        load_sensor.set_reference_unit(92)
        load_sensor.reset()
        load_sensor.tare()
        load_sensor.power_up()
        self.load_sensor = load_sensor
        self.queue = Queue(10)
        self.process = Process(target=self.read_data)
        self.process.start()

    def stop(self):
        self.load_sensor.power_down()

    def get_weight(self):
        return self.queue.get()

    def read_data(self):
        while True:
            weight = self.load_sensor.get_weight(1)
            self.queue.put(weight)
