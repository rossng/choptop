import threading
from collections import deque
from hx711 import HX711
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

    def start(self):
        self.buffer = deque([], maxlen=10)
        #self.thread = threading.Thread(target = self.collectStream)
        #self.thread.start()

    def stop(self):
        self.thread.join()
        self.load_sensor.power_down()

    def getWeight(self):
        return self.load_sensor.get_weight(1)
    
    def update(self):
        weight = self.load_sensor.get_weight(1)
        self.buffer.appendleft(weight)

    def collectStream(self):
        try:
            while True:
                weight = self.load_sensor.get_weight(1)
                self.buffer.appendleft(weight)
        except:
            self.load_sensor.power_down()
            GPIO.cleanup() 