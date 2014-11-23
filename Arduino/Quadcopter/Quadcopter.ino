#include <Servo.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <PID_v1.h>
#include "geometry.h"
#include "magnetometer.h"
#include "accelerometer.h"
Servo fl, fr, bl, br;
LiquidCrystal_I2C lcd(0x3F, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);
Magnetometer mag(0x1E);
Accelerometer acc(0x53);

#define MAGNETOMETER 0x1E
#define ACCELEROMETER 0x53
#define GYROSCOPE 0x69
#define BAROMETER 0x77

void into_buffer(geometry::Vector value, char buffer[20]) {
  dtostrf(value.getX(), 6, 2, buffer);
  buffer[6] = ' ';
  dtostrf(value.getY(), 6, 2, buffer + 7);
  buffer[13] = ' ';
  dtostrf(value.getZ(), 6, 2, buffer + 14);
}

geometry::Rotation rot = geometry::Rotation(geometry::Vector(0,0,1), 0);
long last_time;

void setup() {
  lcd.begin(20, 4);
  lcd.clear();
  lcd.home();
  Serial2.begin(19200);
  while(true) {
 // Serial2.write("Howdy partner");
    if(Serial2.available()) {
      int val = Serial2.read();
      lcd.write(val);
      Serial2.write(val);
    }
  }
   pinMode(13, OUTPUT); 
  fl.attach(44);
  fl.writeMicroseconds(1000);
  delay(2000);
  //while(true) {
  int pos;
  for(pos = 1100; pos < 1200; pos += 1) {
    fl.writeMicroseconds(pos);
    lcd.home();
    lcd.print(pos);
    delay(50);
  }
  fl.writeMicroseconds(1500);
  delay(1000);
  /*for(; pos > 1200; pos -=1) {
    fl.writeMicroseconds(pos);
    lcd.home();
    lcd.print(pos);
    delay(50);
  }*/
 // 1} 
 fl.writeMicroseconds(1000);
  mag.init();
  //mag.calibrate();
  acc.init();
  Wire.beginTransmission(GYROSCOPE);
  Wire.write(0x20); // Start at reg 20
  Wire.write(0x0F); // Set enable
  Wire.endTransmission();
  last_time = millis();
}

void loop() {
  if(millis() - last_time < 100) return;
  Wire.beginTransmission(GYROSCOPE);
  Wire.write(0xA8);
  Wire.endTransmission();
  Wire.requestFrom(GYROSCOPE, 6, false);
  short gyro_x = Wire.read() + (Wire.read() << 8);
  //Wire.beginTransmission(GYROSCOPE);
  //Wire.write(0x29);
  //Wire.endTransmission();
  //Wire.requestFrom(GYROSCOPE, 2, false);
  short gyro_y = Wire.read() + (Wire.read() << 8);
  //Wire.beginTransmission(GYROSCOPE);
  //Wire.write(0x2A);
  //Wire.endTransmission();
  //Wire.requestFrom(GYROSCOPE, 2, false);
  short gyro_z = Wire.read() + (Wire.read() << 8);
  
  lcd.home();
  double multi = (millis() - last_time)/1000.0 * 0.00875;
  last_time = millis();
  rot = rot * geometry::Rotation(geometry::Vector(1,0,0), multi * gyro_x * DEG_TO_RAD) * geometry::Rotation(geometry::Vector(0,1,0), multi * gyro_y * DEG_TO_RAD) *  geometry::Rotation(geometry::Vector(0,0,1), multi * gyro_z * DEG_TO_RAD);
  char buffer[24];
  /*dtostrf(multi * gyro_x, 20, 16, buffer);
  lcd.print(buffer);
  dtostrf(multi * gyro_y, 20, 16, buffer);
  lcd.print(buffer);
  dtostrf(multi * gyro_z, 20, 16, buffer);
  lcd.print(buffer);
  dtostrf(rot.get_k(), 20, 16, buffer);
  lcd.print(buffer);
  return;*/
  dtostrf(rot.get_real(), 20, 16, buffer);
  lcd.print(buffer);
  dtostrf(rot.get_j(), 20, 16, buffer);
  lcd.print(buffer);
  dtostrf(rot.get_i(), 20, 16, buffer);
  lcd.print(buffer);
  dtostrf(rot.get_k(), 20, 16, buffer);
  lcd.print(buffer);
  return;/*
  char buffer[20];
  geometry::Vector value = mag.read();
  lcd.home();
  dtostrf(value.getX(), 6, 2, buffer);
  buffer[6] = ' ';
  dtostrf(value.getY(), 6, 2, buffer + 7);
  buffer[13] = ' ';
  dtostrf(value.getZ(), 6, 2, buffer + 14);
  lcd.print(buffer);
  /*lcd.setCursor(0,1);
  dtostrf(value.magnitude(), 18, 10, buffer);
  lcd.print(buffer);
  lcd.setCursor(0,2);
  dtostrf(mag.calibrationData.getX() * 1000, 6, 2, buffer);
  buffer[6] = ' ';
  dtostrf(mag.calibrationData.getY() * 1000, 6, 2, buffer + 7);
  buffer[13] = ' ';
  dtostrf(mag.calibrationData.getZ() * 1000, 6, 2, buffer + 14);
  lcd.print(buffer);
  lcd.setCursor(0,3);
  dtostrf(mag.calibrationData.magnitude(), 18, 10, buffer);
  lcd.print(buffer);
  
  return;*//*
  geometry::Vector accel = acc.read();
  lcd.setCursor(0, 1);
  dtostrf(accel.getX(), 6, 2, buffer);
  buffer[6] = ' ';
  dtostrf(accel.getY(), 6, 2, buffer + 7);
  buffer[13] = ' ';
  dtostrf(accel.getZ(), 6, 2, buffer + 14);
  lcd.print(buffer);
  Wire.beginTransmission(GYROSCOPE);
  Wire.write(0x28);
  Wire.endTransmission();
  Wire.requestFrom(GYROSCOPE, 2, false);
  short gyro_x = Wire.read() + (Wire.read() << 8);
  Wire.beginTransmission(GYROSCOPE);
  Wire.write(0x29);
  Wire.endTransmission();
  Wire.requestFrom(GYROSCOPE, 2, false);
  short gyro_y = Wire.read() + (Wire.read() << 8);
  Wire.beginTransmission(GYROSCOPE);
  Wire.write(0x2A);
  Wire.endTransmission();
  Wire.requestFrom(GYROSCOPE, 2, false);
  short gyro_z = Wire.read() + (Wire.read() << 8);
  Wire.beginTransmission(BAROMETER);
  Wire.write(0xF4);
  Wire.write(0x2E);
  Wire.endTransmission();
  Wire.beginTransmission(BAROMETER);
  Wire.write(0xF6);
  Wire.endTransmission();
  Wire.requestFrom(BAROMETER, 2, false);
  short temp = (Wire.read() << 8) + Wire.read();
  lcd.setCursor(0, 2);
  sprintf(buffer, "%+5d %+5d %+5d", gyro_x/1000, gyro_y/1000, gyro_z/1000);
  lcd.print(buffer);
  lcd.setCursor(0, 3);
  sprintf(buffer, "%+5d", temp);
  lcd.print(buffer);
  delay(50);*/
}
