from sensor import Sensor
def main():
    # my code here
    sensor_1 = Sensor(20, 21)

    while True:
        weight = sensor_1.get_weight()
        print weight

if __name__ == "__main__":
    main()