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
Gyroscope gyro(0x69);

void setup() {
  // Set up the LCD screen
  lcd.begin(20, 4);
  lcd.clear();
  lcd.home();
  // Attach ports to the motor outputs.
  fl.attach(44);
  fr.attach(45);
  bl.attach(46);
  br.attach(47);
  // Initialise Sensors
  mag.init();
  acc.init();
  gyro.init();
}

void loop() {
  
}
