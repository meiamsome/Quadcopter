#ifndef SENSOR
#define SENSOR

/*
  Base Sensor class to be used to interface all sensors.
 */
template <class T> class Sensor{
  public:
    /*
      Initialize the sensor, this includes butting it in the correct mode.
     */
    virtual void init() = 0;
    /*
      Run a calibration of the sensor. The current calibration data should be erased and new data obtained
     */
    virtual void calibrate() = 0;
    /* 
      Methods to get and set the calibration data.
     */
    virtual T get_calibration_data() = 0;
    virtual void set_calibration_data(T data) = 0;
    /*
      Query the sensor for data.
     */
    virtual T read() = 0;
};
#endif
