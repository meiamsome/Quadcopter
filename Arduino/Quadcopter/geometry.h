#ifndef GEOMETRY
#define GEOMETRY

namespace geometry {
  /*
    Represents a vector in R3.
   */
  class Vector {
    public:
      Vector(double x, double y, double z);
      double magnitude();
      double getX();
      double getY();
      double getZ();
      Vector normalise();
      Vector operator+(Vector other);
      Vector operator-(Vector other);
      Vector operator*(Vector other);
      Vector operator*(double scale);
      Vector operator/(Vector other);
      Vector operator/(double scale);
      void operator+=(Vector other);
    private:
      double x, y, z;
  };
  
  /*
    Quarternion Rotation.
   */
  class Rotation {
    public:
      Rotation(Vector axis, double angle);
      Rotation(double real, double i, double j, double k);
      double get_real();
      double get_i();
      double get_j();
      double get_k();
      Rotation operator*(Rotation other);
      
    //private:
      double real;
      Vector imaginary{0,0,0};
  };
};
#endif
