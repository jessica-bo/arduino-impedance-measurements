#include <Math.h>
#include "IA.h"
// will print debug log messages to Serial Monitor if set to true
#define DEBUG false

unsigned const long START_FREQ = 5000;
unsigned const long FREQ_INCR = 5000;
unsigned const int NUM_INCR = 20;
unsigned const long REF_RESIST = 100000;
const byte N_SAMPLES = 50;
const byte BUTTON_PIN = 8;

double gain[NUM_INCR];

void setup() {
  
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT);
  

  Serial.begin(9600);
  IA::init(START_FREQ, NUM_INCR, FREQ_INCR, REF_RESIST, DEBUG);

  IA::startSweep();
  int real, imag;
  int num_gain_samples = 5;
  for (int ni = 0; ni < NUM_INCR; ni++) {
    double c_gain = 0;
    for (int gi = 0; gi < num_gain_samples; gi++) {      
      IA::readReadImag(&real, &imag);
      c_gain += IA::calcGain(real, imag, REF_RESIST);
      IA::repeatFreq();
    }
    gain[ni] = c_gain / num_gain_samples;
    IA::nextFreq();
  }
  
  Serial.println("...............DONE SETUP AND CALIBRATION...............");
}

void loop() {
  // light up LED to indicate calibration done
  digitalWrite(LED_BUILTIN, HIGH);
  waitForButton();
  digitalWrite(LED_BUILTIN, LOW);

  IA::startSweep();
  int real, imag;
  unsigned long curr_freq = START_FREQ;
  for (int ni = 0; ni < NUM_INCR; ni++) {
    for (int ns = 0; ns < N_SAMPLES; ns++) {
      IA::readReadImag(&real, &imag);
      double imp = IA::calcImpedance(real, imag, gain[ni]);
      printRow(curr_freq, real, imag, imp, gain[ni]);
      IA::repeatFreq();
    }
    IA::nextFreq();
    curr_freq += FREQ_INCR;
    delay(1000);
  }
  Serial.println("...............DONE SWEEPING...............");
}

void ledOn() {
  digitalWrite(LED_BUILTIN, HIGH);
}

void ledOff() {
  digitalWrite(LED_BUILTIN, LOW);
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
