#include <Wire.h>
#include "geometry.h"
#include "sensor.h"


/*
  This class provides communication to the L3G4200D i2c gyroscope returned as a vector.
  TODO: change this to be in geometry::Rotation
 */
class Gyroscope: Sensor<geometry::Rotation> {
  public:
    Gyroscope(char i2c_address);
    void init();
    void calibrate();
    geometry::Rotation get_calibration_data();
    void set_calibration_data(geometry::Rotation data);
    geometry::Rotation read();
  private:
    double readValue();
    char address;
    geometry::Rotation calibrationData{1, 0, 0, 0};
};
