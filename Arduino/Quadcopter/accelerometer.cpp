#include "geometry.h"
#include "accelerometer.h"



Accelerometer::Accelerometer(char i2c_address) {
  address = i2c_address;
}

void Accelerometer::init() {
  Wire.beginTransmission(address);
  Wire.write(0x2C); // Start at reg 2C
  Wire.write(0x0F); // Run at max samples/sec
  Wire.write(0x08); // Set measure bit
  Wire.endTransmission();
  Wire.beginTransmission(address);
  Wire.write(0x31); // Measurement Settings
  Wire.write(0x0B); // Full res 16g mode
  Wire.endTransmission();
}
void Accelerometer::calibrate() {
  // TODO
}

geometry::Vector Accelerometer::get_calibration_data() {
  return calibrationData;
}

void Accelerometer::set_calibration_data(geometry::Vector data) {
  calibrationData = data;
}

geometry::Vector Accelerometer::read() {
  Wire.beginTransmission(address);
  Wire.write(0x32);
  Wire.endTransmission();
  Wire.requestFrom(address, 6, false);
  return geometry::Vector(readValue(), readValue(), readValue()) * this -> calibrationData;
}

double Accelerometer::readValue() {
  return (double) Wire.read() + (Wire.read() << 8);
}
