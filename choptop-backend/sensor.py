import threading
import Queue
from hx711 import HX711
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
        self.queue = Queue.Queue()
        self.thread = threading.Thread(target = self.collectStream)
        self.thread.start()

    def stop(self):
        self.thread.join()
        self.load_sensor.power_down()
        
    def getWeight(self):
        return self.load_sensor.get_weight(1)
    
    def collectStream(self):
        while True:
            weight = self.load_sensor.get_weight(1)
            self.queue.put(weight)

