#include "gyroscope.h"
#include <Wire.h>
#include <Arduino.h>

Gyroscope::Gyroscope(char i2c_address) {
  this -> address = i2c_address;
}

void Gyroscope::init() {
  Wire.beginTransmission(this -> address);
  Wire.write(0x20); // Start at reg 20
  Wire.write(0x0F); // Set enable
  Wire.endTransmission();
}

double Gyroscope::readValue() {
  return Wire.read() + (Wire.read() << 8);
}

void Gyroscope::calibrate() {
  // TODO
}

geometry::Rotation Gyroscope::get_calibration_data() {
  return calibrationData;
}

void Gyroscope::set_calibration_data(geometry::Rotation data) {
  calibrationData = data;
}

geometry::Rotation Gyroscope::read() {
  Wire.beginTransmission(address);
  Wire.write(0xA8);
  Wire.endTransmission();
  Wire.requestFrom(address, 6, false);
  return geometry::Rotation(geometry::Vector(1,0,0), readValue() * DEG_TO_RAD)
   * geometry::Rotation(geometry::Vector(0,1,0), readValue() * DEG_TO_RAD)
   * geometry::Rotation(geometry::Vector(0,0,1), readValue() * DEG_TO_RAD);;
}
