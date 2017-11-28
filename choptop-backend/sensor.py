import threading
from collections import deque

import time

from hx711 import HX711
from multiprocessing import Process, Queue, Pool
from threading import Timer
import RPi.GPIO as GPIO


class Sensor:
    def __init__(self, dout, pd_sck, m, c):
        load_sensor = HX711(dout, pd_sck)
        load_sensor.set_reading_format("LSB", "MSB")
        load_sensor.set_reference_unit(92)
        load_sensor.reset()
        load_sensor.tare()
        load_sensor.power_up()
        self.load_sensor = load_sensor
        self.queue = Queue(3)
        self.process = Process(target=self.read_data)
        self.multiplier = m
        self.offset = c

    def start(self):
        self.process.start()

    def stop(self):
        self.load_sensor.power_down()

    def get_weight(self):
        return self.queue.get()

    def read_data(self):
        while True:
            weight = self.multiplier * self.load_sensor.get_weight(1) + self.offset
            self.queue.put(weight - self.offset)
