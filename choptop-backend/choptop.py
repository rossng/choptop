from sensor import Sensor
import RPi.GPIO as GPIO
import time, sched
import sys

class ChopTop:
	def __init__(self):
		self.finger_position = (0,0)
	def start(self):
		sensor_1 = Sensor(20, 21)
		sensor_1.start()
		time.sleep(0.5)
		frame_time_millis = int((1/80)*1000)
		old_stdout = sys.stdout
		timestr = time.strftime("%Y%m%d-%H%M%S")
		timestr = timestr + '.log'
		log_file = open(timestr,"w")
		sys.stdout = log_file
        self.scheduler = sched.scheduler(time.time, time.sleep)
        self.scheduler.enter(10, 1, update, (self, sc))
        self.scheduler.run()

def update(self, sc):
    try:
        sensor_1.update()
        #get weights for each sensor
        if sensor_1.buffer.count > 0:
            weight = sensor_1.buffer.pop()
        print weight + ", " + int(round(time.time() * 1000))
        self.finger_position = calculatePosition([weight])
        self.scheduler.enter(10, 1, update, (self, sc))
        self.scheduler.run()
    except:
        GPIO.cleanup()

def calculatePosition(weights):
	return (0,0)

def main():
	# my code here
	choptop = ChopTop()
	choptop.start()

if __name__ == "__main__":
		main()
	