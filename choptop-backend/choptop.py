from sensor import Sensor
import RPi.GPIO as GPIO
import time

class ChopTop:
    def __init__(self):
        self.finger_position = (0,0)
    def main(self):
        sensor_1 = Sensor(20, 21)
        sensor_1.start()
        time.sleep(0.5)
        try:
            while True:
                #get weights for each sensor
                if sensor_1.buffer.count > 0:
                    weight = sensor_1.buffer.pop()
                print weight
                self.finger_position = calculatePosition([weight])
        except:
            GPIO.cleanup() 

def calculatePosition(weights):
    return (0,0)

def main():
    # my code here
    choptop = ChopTop()
    choptop.main()

if __name__ == "__main__":
        main()
    