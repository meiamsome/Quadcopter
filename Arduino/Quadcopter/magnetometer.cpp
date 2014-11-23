#include "magnetometer.h"
#include <Wire.h>
#include <Arduino.h>

Magnetometer::Magnetometer(char i2c_address) {
  this -> address = i2c_address;
}

void Magnetometer::init() {
  Wire.beginTransmission(this -> address);
  Wire.write(0x00); // Start at Reg 0
  Wire.write(0x70);
  Wire.write(0xA0);
  Wire.write(0x00);
  Wire.endTransmission();
}

double readValue() {
  return (double) (Wire.read() << 8) + Wire.read();
}

void Magnetometer::calibrate() {
  int loops = 100;
  int samples_per_loop = 100;
  this -> calibrationData = geometry::Vector(0,0,0);
  this -> init();
  Wire.beginTransmission(this -> address);
  Wire.write(0x00);
  Wire.endTransmission();
  Wire.requestFrom(address, 1, false);
  char data = Wire.read() & 0xFC;
  for (int j = 0; j < loops; j += 1) {
    Wire.beginTransmission(this -> address);
    Wire.write(0x00); // Start at Reg 0
    Wire.write(data);
    Wire.endTransmission();
    geometry::Vector first_reading(0,0,0);
    for(int i = 0; i < samples_per_loop; i++) {
      Wire.beginTransmission(this -> address);
      Wire.write(0x03);
      Wire.endTransmission();
      delay(10);
      Wire.requestFrom(address, 6, false);
      first_reading += geometry::Vector(readValue(), readValue(), readValue());
    }

    Wire.beginTransmission(this -> address);
    Wire.write(0x00); // Start at Reg 0
    Wire.write(data | 1); // Self test (Causes a 1.1 Guass to be applied to all axis)
    Wire.endTransmission();
    geometry::Vector second_reading(0,0,0);
    for(int i = 0; i < samples_per_loop; i++) {
      Wire.beginTransmission(this -> address);
      Wire.write(0x03);
      Wire.endTransmission();
      delay(10);
      Wire.requestFrom(address, 6, false);
      second_reading += geometry::Vector(readValue(), readValue(), readValue());
    }
    this -> calibrationData += geometry::Vector(1.1,1.1,1.1) / ((second_reading - first_reading) / samples_per_loop);
  }
  this -> calibrationData = this -> calibrationData / loops;
  this -> init();

}

geometry::Vector Magnetometer::get_calibration_data() {
  return calibrationData;
}

void Magnetometer::set_calibration_data(geometry::Vector data) {
  calibrationData = data;
}

geometry::Vector Magnetometer::read() {
  Wire.beginTransmission(address);
  Wire.write(0x03);
  Wire.endTransmission();
  Wire.requestFrom(address, 6, false);
  return geometry::Vector(readValue(), readValue(), readValue()) * this -> calibrationData;
}
