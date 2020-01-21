#include <Wire.h> // I2C Arduino Library
#include <Math.h>
#include "IA.h"

#define FREQ  (10000)
#define REF_RESIST  (10000)
// will print debug log messages to Serial Monitor if set to true
#define DEBUG true

void setup() {
  Wire.begin();
  Serial.begin(9600);
  Serial.println("AD5933 Test Started!");
  IA::init(FREQ, REF_RESIST, DEBUG);
}

void loop() {
  // get the moving average of n points
  int n = 100;
  double imp;
  IA::readImp(&imp);
  double avg = imp;
  double maximp = imp;
  for (int i = 2; i < n+1; i++) {
    IA::readImp(&imp);
    avg = avg + (imp-avg)/i;
    maximp = fmax(maximp, imp);
  }
  Serial.println(avg);
  Serial.println(maximp);
}
