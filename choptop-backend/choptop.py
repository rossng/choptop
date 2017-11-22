from sensor import Sensor
import RPi.GPIO as GPIO
import time
import sched
import sys


class ChopTop:

    def __init__(self):
        self.finger_position = (0, 0)

    def start(self):
        self.sensor_1 = Sensor(20, 21)
        self.sensor_1.start()
        time.sleep(0.5)
        frame_time_millis = int((1 / 80) * 1000)
        self.old_stdout = sys.stdout
        timestr = time.strftime("%Y%m%d-%H%M%S")
        timestr = timestr + '.log'
        self.log_file = open(timestr, "w")
        sys.stdout = self.log_file
        self.scheduler = sched.scheduler(time.time, time.sleep)
        self.scheduler.enter(10, 1, self.update, (self,))
        try:
            self.scheduler.run()
        except:
            GPIO.cleanup()
            self.log_file.close()
            sys.stdout = self.old_stdout

    def update(self):
        self.sensor_1.update()
        # get weights for each sensor
        if self.sensor_1.buffer.count > 0:
            weight = sensor_1.buffer.pop()
            print weight
            print ", "
            print int(round(time.time() * 1000))
        self.finger_position = calculatePosition([weight])
        self.scheduler.enter(10, 1, self.update, (self,))


def calculatePosition(weights):
    return (0, 0)


def main():
    # my code here
    choptop = ChopTop()
    choptop.start()

if __name__ == "__main__":
    main()
