#include "IRSensor.h"
#include <Arduino.h>

IRSensor::IRSensor(int pin, double powerfitA, double powerfitB, int analogBits,
                   double analogMaxVoltage, unsigned int numSamples,
                   unsigned int measureTimeMs)
    : pin(pin), powerfitA(powerfitA), powerfitB(powerfitB),
      numSamples(numSamples), measureTimeMs(measureTimeMs) {

  // Set pin mode to input. This is not required on some boards as the analog
  // is the only allowed mode, but it's still set in case another board is
  // used where other modes are allowed.
  pinMode(pin, INPUT);

  // Initialize the vals vector
  vals.resize(numSamples);

  // Calculate the analog-to-voltage conversion coefficient
  analogToVoltage = analogMaxVoltage / (pow(2.0, analogBits) - 1);
}

int IRSensor::read() {
  // Read multiple measurements and save them in the vector
  for (int i = 0; i < numSamples; i++) {
    vals[i] = analogRead(pin);

    // Delay between successive measurements to read different values
    delay(measureTimeMs);
  }

  // Find the mode of the analog readings (the most repeated value) and convert
  // it to voltage
  double voltage = mode(vals) * analogToVoltage;

  // Use the voltage in a power function (whose parameters are provided by the 
  // user to map voltage to distance) and return the value as int.
  return powerfitA * pow(voltage, powerfitB);
}

int IRSensor::mode(std::vector<int> vals) {
  // Construct a map that maps each value to the number of its repetitions
  std::map<int, int> valmap;
  for (int i = 0; i < numSamples; i++) {
    valmap[vals[i]]++;
  }
  // Find the key that corresponds to the highest repetitions
  return maxkey(valmap);
}

int IRSensor::maxkey(std::map<int, int> &valmap) {
  int best;
  int best_count = 0;
  // For each mapping pair (key-value), if the value is greater than what is
  // found thus far, then save the key as the best thus far
  for (auto const &p : valmap) {
    if (p.second > best_count) {
      best_count = p.second;
      best = p.first;
    }
  }
  return best;
}
