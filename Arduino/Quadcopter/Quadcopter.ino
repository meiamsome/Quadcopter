#include <Servo.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <PID_v1.h>
#include "geometry.h"
#include "magnetometer.h"
#include "accelerometer.h"
#include "gyroscope.h"
Servo fl, fr, bl, br;
LiquidCrystal_I2C lcd(0x3F, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);
Magnetometer mag(0x1E);
Accelerometer acc(0x53);
Gyroscope gyro(0x69);

boolean primed = false;

void prime() {
  fl.writeMicroseconds(1000);
  fr.writeMicroseconds(1000);
  bl.writeMicroseconds(1000);
  br.writeMicroseconds(1000);
  delay(2);
  primed = true;
}
double flbroff = 0, frbloff = 0;
double flbrpow = 1, frblpow = 1;
double kp = 2, ki = 3, kd = 1;
double sp = 0;
PID flbrpid(&flbroff, &flbrpow, &sp, kp, ki, kd, REVERSE);
PID frblpid(&frbloff, &frblpow, &sp, kp, ki, kd, REVERSE);
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
  Serial1.begin(19200);
  flbrpid.SetOutputLimits(-100, 100);
  flbrpid.SetMode(AUTOMATIC);
  frblpid.SetOutputLimits(-100, 100);
  frblpid.SetMode(AUTOMATIC);
  prime();
}
geometry::Vector old1 = geometry::Vector(0,0,0);
geometry::Vector old2 = geometry::Vector(0,0,0);
geometry::Vector old3 = geometry::Vector(0,0,0);
geometry::Vector old4 = geometry::Vector(0,0,0);
geometry::Vector old5 = geometry::Vector(0,0,0);
int power = 00;
float factor = 100;
char out_string[1024];
int i = 0;

void writeStatus() {
  String status = "0:";
  status += millis();
  status += " ";
  status += (primed?"1":"0");
  status +=  " ";
  char data[20];
  dtostrf(old1.getX(), 5, 2, data);
  status += data;
  status += " ";
  dtostrf(old1.getY(), 5, 2, data);
  status += data;
  status += " ";
  dtostrf(old1.getZ(), 5, 2, data);
  status += data;
  status += " ";
  Serial1.println(status);
}
char inbuffer[100];
int pos = 0;
void loop() {
  if(Serial1.available()) {
    char in = Serial1.read();
    if(in == '\n') {
      inbuffer[pos] = 0;
      //process line now
      if(inbuffer[0] == 'd') {
        sscanf(inbuffer, "d%i %lf %lf", &power, &frbloff, &flbroff);
        String status = "2:";
        status += power;
        status += " ";
        char data[20];
        dtostrf(frbloff, 5, 2, data);
        status += data;
        status += " ";
        dtostrf(flbroff, 5, 2, data);
        status += data;
        Serial1.println(status);
      }
      pos = 0;
    } else inbuffer[pos++] = in;
  }
  //if(primed) {
    old5 = old4;
    old4 = old3;
    old3 = old2;
    old2 = old1;
    old1 = acc.read();
    geometry::Vector aval = (old1 + old2 + old3 + old4 + old5).normalise();
    //double rot1 = acos(-aval.getZ());
    //double rot2 = atan2(aval.getX(), aval.getY());
    //double pwr = -tan(rot1);
    /*floff = cos(rot2 - PI/4) * pwr;
    froff = cos(rot2 + PI/4) * pwr;
    bloff = cos(rot2 - 3*PI/4) * pwr;
    broff = cos(rot2 + 3*PI/4) * pwr;*/
    //flbrpid.Compute();
    //frblpid.Compute();
    int fl_actu = primed?1000:0, fr_actu = fl_actu, bl_actu = fl_actu, br_actu = fl_actu;
    if(power > 0) {
      int pow = map(power, 0, 100, 0, 400);
      fl_actu = 1200 + pow;// + pow * flpow/400.0;// 1200 + pow + flpow???
      fr_actu = 1200 + pow;// + pow;// * frpow/400.0;
      bl_actu = 1200 + pow;// + pow;// * blpow/400.0;
      br_actu = 1200 + pow;// + pow;// * brpow/400.0;
    }
    if(!(i ++ % 20)) {
      writeStatus();
    }
    fl.writeMicroseconds(fl_actu);
    fr.writeMicroseconds(fr_actu);
    bl.writeMicroseconds(bl_actu);
    br.writeMicroseconds(br_actu);
  //}
}
