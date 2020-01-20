#include <Wire.h> // I2C Arduino Library
#include "AD5933.h" // C++ library for AD5933 impedance analyzer chip
#include "IA.h"
#define address 0x0D //I2C 7bit address

// range from: 0.1Hz to
#define START_FREQ  (10000)
#define FREQ_INCR   (500)//(1000)
// ranging from: 1 ~ 511
#define NUM_INCR    (40)
#define REF_RESIST  (10000)

double gain[NUM_INCR + 1];
int phase[NUM_INCR + 1];

void setup() {
  Wire.begin();
  Serial.begin(9600);
  Serial.println("AD5933 Test Started!");
  IA::init(START_FREQ, REF_RESIST);
}

void loop() {
  double imp;
  int currentFreq = START_FREQ;
  while (1) {
      IA::readImp(&imp);
      Serial.println(imp);
  }
//  for (int n = 0; n < NUM_INCR; n++) {
//    Serial.print("reading: ");
//    Serial.print(currentFreq);
//    Serial.print("\n");
//    IA::setFreq(currentFreq);
//    for (int i = 0; i < 20; i++) {
//      IA::readImp(&imp);
//      Serial.println(imp);
//    }
//    currentFreq += FREQ_INCR;
//    delay(1000);
//  }
//  delay(5000);
}
