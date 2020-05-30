#ifndef IRSENSOR_H
#define IRSENSOR_H

#include <map>
#include <vector>

/**
   This class provides precise readings from infrared sensors. It reads multiple
   successive measurements and elects the mode (the most repeated measurement)
   as the right one. It uses user-provided polyfit values a and b for a power
   function (y = a * x ^ b) to convert voltage readings to distance. The unit
   of distance depends on the user-provided values.
*/
class IRSensor {
public:
  /**
     Initializes the object.

     The constructor sets the pin mode to input (for boards with multi-mode
     analog pins).

     The two values a and b in the power equation:
         y = a * x ^ b
     are required to calculate the distance (y) from the voltage (x). These
     values may be deduced by using a graphing tool to graph voltage versus
     distance and fitting a power function.

     @param pin the analog pin to attach the IR sensor to.
     @param powerfitA the a value of the powerfit equation.
     @param powerfitB the b value of the powerfit equation.
     @param numSamples the number of samples to read before deciding the correct
            reading.
     @param analogBits the number of bits in the analog input.
     @param analogMaxVoltage the maximum voltage the analog input can read.
     @param measureTimeMs the time in milliseconds between two different
                          readings.
  */
  IRSensor(int pin, double powerfitA, double powerfitB, int analogBits = 10,
           double analogMaxVoltage = 5.0, unsigned int numSamples = 15,
           unsigned int measureTimeMs = 50);

  /**
     Reads a value from the sensor.

     @return the best guess of the correct value from multiple readings
             (the number of readings is set in the constructor).
  */
  int read();

private:
  /**
     The input analog pin the sensor is connected to.
  */
  int pin;
  double powerfitA, powerfitB, analogToVoltage;
  unsigned int numSamples, measureTimeMs;

  /**
     A list to hold multiple readings from the sensor.
  */
  std::vector<int> vals;

  /**
     Finds the most repeated value in `vals`.

     @param vals the values to find the mode thereof.

     @return the most repeated value.
  */
  int mode(std::vector<int> vals);

  /**
     Finds the key of the maximum value in the given map.

     @param valmap the address of a key-value where it is desired to find the
                   key corresponding to the maximum value.
     
     @return the key of the maximum value.
  */
  int maxkey(std::map<int, int> &valmap);
};

#endif
