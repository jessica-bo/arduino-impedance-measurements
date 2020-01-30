#include <Wire.h> // I2C Arduino Library
#include <Math.h>
#include "IA.h"
// will print debug log messages to Serial Monitor if set to true
#define DEBUG false
// Global variables //
//
//int n = 20;
//double avg[n];

long freqIntervals[] = {1000, 10000, 20000, 30000, 40000, 50000, 60000, 70000, 80000, 90000, 100000};
const byte nIntervals = 11;
//long freqIntervals[] = {10000, 20000, 30000, 40000};
//const byte nIntervals = 4;
const byte nSamples = 50;
const byte BUTTON_PIN = 8;

void setup() {
  const unsigned long REF_RESIST = 100000;
  
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT);
  
  Wire.begin();
  Serial.begin(9600);
//  Serial.println("AD5933 Test Started!");
//  Serial.print("Freq: ");
//  Serial.print(freqIntervals[0]);
//  Serial.println(" Hz");
  IA::init(freqIntervals[0], REF_RESIST, DEBUG);
}

void loop() {
  double gain[nIntervals];
  digitalWrite(LED_BUILTIN, LOW);
  
  for (int ni = 0; ni < nIntervals; ni++) {
    IA::setFreq(freqIntervals[ni]);
    gain[ni] = IA::getGain();
  }

  // light up LED to indicate calibration done
  digitalWrite(LED_BUILTIN, HIGH);
  waitForButton();
  digitalWrite(LED_BUILTIN, LOW);

  int real, imag;
  for (int ni = 0; ni < nIntervals; ni++) {
    IA::setFreq(freqIntervals[ni]);
    IA::setGain(gain[ni]);
    for (int ns = 0; ns < nSamples; ns++) {
      IA::readReadImag(&real, &imag);
      double magnitude = sqrt(pow(real, 2) + pow(imag, 2));
      double imp = 1 / (magnitude * gain[ni]);
      printRow(freqIntervals[ni], real, imag, imp, gain[ni]);
    }
  }

//    int n = 100;
//    double imp;
//    IA::readImp(&imp);
//    double avg = imp;
//    double maximp = imp;
//    for (int i = 2; i < n+1; i++) {
//      IA::readImp(&imp);
//      avg = avg + (imp-avg)/i;
//      maximp = fmax(maximp, imp);
//    }
//    
//    Serial.print("avg impedance =");
//    Serial.print(avg);
//    Serial.print("\t");
//    Serial.print("\t");
//    Serial.print("max impedance =");
//    Serial.println(maximp);
}

void waitForButton() {
  while (digitalRead(BUTTON_PIN) == 0) {
  }
}

void printRow(long freq, int real, int imag, double imp, double gain) {
      Serial.print(freq);
      Serial.print(",");
      Serial.print(real);
      Serial.print(",");
      Serial.print(imag);
      Serial.print(",");
      Serial.print(imp, 2);
      Serial.print(",");
      Serial.print(gain, 12);
      Serial.print("\n");
}
