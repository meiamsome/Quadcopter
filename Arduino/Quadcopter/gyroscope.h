#include <Wire.h>
#include "geometry.h"
#include "sensor.h"

class Gyroscope: Sensor<geometry::Vector> {
  public:
    Accelerometer(char i2c_address);
    void init();
    void calibrate();
    geometry::Vector get_calibration_data();
    void set_calibration_data(geometry::Vector data);
    geometry::Vector read();
  private:
    double readValue();
    char address;
    geometry::Vector calibrationData{0.0039, 0.0039, 0.0039};
};
