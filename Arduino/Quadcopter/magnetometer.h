#include <Wire.h>
#include "geometry.h"
#include "sensor.h"

class Magnetometer: public Sensor<geometry::Vector> {
  public:
    Magnetometer(char i2c_address);
    void init();
    void calibrate();
    geometry::Vector get_calibration_data();
    void set_calibration_data(geometry::Vector data);
    geometry::Vector read();
  private:
    char address;
    geometry::Vector calibrationData{0.00254, 0.00254, 0.00254};
};
