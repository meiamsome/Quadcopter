#include <Arduino.h>
#include "geometry.h"

namespace geometry {
  Vector::Vector(double x, double y, double z) {
    this->x = x;
    this->y = y;
    this->z = z;
  }

  double Vector::magnitude() {
    return sqrt(sq(this->x) + sq(this->y) + sq(this->z));
  }

  double Vector::getX() {
    return this -> x;
  }

  double Vector::getY() {
    return this -> y;
  }

  double Vector::getZ() {
    return this -> z;
  }

  Vector Vector::operator+(Vector other) {
    return Vector(this -> x + other.getX(), this -> y + other.getY(), this -> z + other.getZ());
  }
  Vector Vector::operator-(Vector other) {
    return Vector(this -> x - other.getX(), this -> y - other.getY(), this -> z - other.getZ());
  }
  Vector Vector::operator*(Vector other) {
    return Vector(this -> x * other.getX(), this -> y * other.getY(), this -> z * other.getZ());
  }
  Vector Vector::operator*(double scale) {
    return Vector(this -> x * scale, this -> y * scale, this -> z * scale);
  }
  Vector Vector::operator/(Vector other) {
    return Vector(this -> x / other.getX(), this -> y / other.getY(), this -> z / other.getZ());
  }
  Vector Vector::operator/(double scale) {
    return Vector(this -> x / scale, this -> y / scale, this -> z / scale);
  }
  void Vector::operator+=(Vector other) {
    this -> x += other.getX();
    this -> y += other.getY();
    this -> z += other.getZ();
  }
  
  Vector Vector::normalise() {
    return *this / (this -> magnitude());
  }
  
  /*
  Rotation class
  */
  Rotation::Rotation(Vector direction, double angle) {
    real = cos(angle/2);
    imaginary = direction.normalise() * sin(angle/2);
  }
  Rotation::Rotation(double real, double i, double j, double k) {
    this -> real = real;
    imaginary = Vector(i,j,k);
  }
  double Rotation::get_real() {
    return real;
  }
  double Rotation::get_i() {
    return imaginary.getX();
  }
  double Rotation::get_j() {
    return imaginary.getY();
  }
  double Rotation::get_k() {
    return imaginary.getZ();
  }
  Rotation Rotation::operator*(Rotation other) {
    return Rotation(
      real * other.get_real() - this -> get_i() * other.get_i() - this->get_j() * other.get_j() - this->get_k() * other.get_k(),
      real * other.get_i() + this -> get_i() * other.get_real() + this->get_j() * other.get_k() - this -> get_k() * other.get_j(),
      real * other.get_j() - this -> get_i() * other.get_k() + this -> get_j() * other.get_real() + this -> get_k() * other.get_i(),
      real * other.get_k() + this -> get_i() * other.get_j() - this -> get_j() * other.get_i() + this -> get_k() * other.get_real()
    );
  }
};


