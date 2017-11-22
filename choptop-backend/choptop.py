from sensor import Sensor
import Queue
import time

class ChopTop:
    def __init__(self):
        self.finger_position = (0,0)
    def main(self):
        sensor_1 = Sensor(20, 21)
        sensor_1.start()
        while True:
            #get weights for each sensor
            weight = sensor_1.queue.get()
            print weight
            self.finger_position = calculatePosition([weight])
            print sensor_1.queue.qsize()
            time.sleep(0.001)

def calculatePosition(weights):
    return (0,0)

def main():
    # my code here
    choptop = ChopTop()
    choptop.main()

if __name__ == "__main__":
        main()
    