import json
import threading
from time import sleep
try:
    import smbus
except ImportError:
    pass

class BatteryMonitor(object):
    # Different module types
    _ADS1015 = 0

    def __init__(self, i2cbus=None, filename="batterymonitor.json", **kwargs):
        if i2cbus is None:
            try:
                i2cbus = smbus.SMBus(1)
            except NameError:
                raise Exception("No i2c bus selected.")
        self.i2cbus = i2cbus
        # Settings default.
        self.settings = {
            "cells": 3,
            "type": BatteryMonitor._ADS1015,
            "address": 0x48,
            "scale": [1, 1, 1],
        }
        self.settings.update(kwargs)
        self.file = filename
        try:
            with open(self.file) as fp:
                self.settings.update(json.load(fp))
        except IOError:
            pass
        # Thread stuff
        self.thread = None
        self.running = False
        self.has_async = False
        self.async_reading = []

    def start_async_reading(self, delay=None):
        """This method should be called to spawn a thread to do the reading.
        If delay is None, a single reading will be taken. Otherwise, repeated readings
        at delay interval will be taken."""
        if self.thread is not None:
            if self.thread.is_alive():
                return False
            else:
                self.thread.join(0)
        self.running = True
        self.has_async = False
        self.async_reading = []
        self.thread = threading.Thread(target=BatteryMonitor._async_thread, args=(self, delay))
        self.thread.start()

    def has_async_reading(self):
        return self.has_async

    def get_async_reading(self):
        self.has_async = False
        return self.async_reading

    def stop_async_reading(self):
        self.running = False

    def _async_thread(self, delay):
        reading = self.get_reading()
        self.has_async, self.async_reading = True, reading
        if delay is None:
            return
        while self.running:
            sleep(delay)
            reading = self.get_reading()
            self.has_async, self.async_reading = True, reading

    def get_reading(self, scale=True):
        """Request a reading from the monitor.
        This method IS blocking and should not be used in a time-sensitive context.
        """
        if scale:
            return [self.settings["scale"][i]  * x for i, x in enumerate(BatteryMonitor._reading_function_dict[self.settings["type"]](self))]
        else:
            return BatteryMonitor._reading_function_dict[self.settings["type"]](self)

    _reading_function_dict = {
        _ADS1015: lambda self: [val*6.144/2048 for val in [
            (val >> 12) | ((val & 0xFF) << 4) for val in [
            [
             self.i2cbus.write_word_data(self.settings["address"], 0x01, 0x0081 + ((x + 1) << 4)),
             sleep(1.0/20),
             self.i2cbus.read_word_data(self.settings["address"], 0x00)
            ][2]
          for x in xrange(self.settings["cells"])]]],
    }

    def calibrate(self):
        #TODO
        raw_input("Press enter to take a reading.")
        reading = self.get_reading(False)
        print("Reading is:")
        print(reading)
        pass

    def save(self):
        try:
            with open(self.file, "w") as fp:
                json.dump(self.settings, fp)
        except IOError as e:
            # TODO: Handle this?
            raise

if __name__ == "__main__":
    monitor = BatteryMonitor(address=0x4A)
    monitor.start_async_reading(1./2)
    try:
        while True:
            if monitor.has_async_reading():
                print(monitor.get_async_reading())
            sleep(1.0/20)
    except KeyboardInterrupt:
        monitor.stop_async_reading()
        monitor.save()
        raise