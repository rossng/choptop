class Sensor:
    def __init__(self, dout, pd_sck):
    load_sensor = HX711(dout, pd_sck)
    load_sensor.set_reading_format("LSB", "MSB")
    load_sensor.set_reference_unit(92)
    load_sensor.reset()
    load_sensor.tare()
    load_sensor.power_up()
    self.load_sensor = load_sensor

    def get_weight(self):
        return self.get_weight(1)

