#ifndef SENSOR
#define SENSOR
template <class T> class Sensor{
  public:
    virtual void init() = 0;
    virtual void calibrate() = 0;
    virtual T get_calibration_data() = 0;
    virtual void set_calibration_data(T data) = 0;
    virtual T read() = 0;
};
#endif
