#include <Math.h>
#include "IA.h"
// will print debug log messages to Serial Monitor if set to true
#define DEBUG false

unsigned const long START_FREQ = 5000;
unsigned const long FREQ_INCR = 5000;
unsigned const int NUM_INCR = 20;
unsigned const long REF_RESIST = 100000;
const byte N_SAMPLES = 10;
const byte BUTTON_PIN = 8;

double gain[NUM_INCR];
double sys_phase[NUM_INCR];

const char* csv_delimiter = "\t";

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
    sys_phase[ni] = atan2(imag, real);
    IA::nextFreq();
  }

  
  Serial.println("...............DONE SETUP AND CALIBRATION...............");
}

void loop() {
  // light up LED to indicate calibration done
  ledOn();
  waitForButton();
  ledOff();

  IA::startSweep();
  int sys_real, sys_imag;
  unsigned long curr_freq = START_FREQ;
  for (int ni = 0; ni < NUM_INCR; ni++) {
    for (int ns = 0; ns < N_SAMPLES; ns++) {
      IA::readReadImag(&sys_real, &sys_imag);
      double imp = IA::calcImpedance(sys_real, sys_imag, gain[ni]);
      // calculate the real phase by offsetting the system induced phase
      double phase = atan2(sys_real, sys_imag) - sys_phase[ni];
      double real = imp * cos(phase);
      double imag = imp * sin(phase);
      printRow(curr_freq, real, imag, phase, imp, gain[ni]);
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

void printHeader() {
      Serial.print("Freq");
      Serial.print(csv_delimiter);
      Serial.print("Real");
      Serial.print(csv_delimiter);
      Serial.print("Imag");
      Serial.print(csv_delimiter);
      Serial.print("Phase");
      Serial.print(csv_delimiter);
      Serial.print("Impedance");
      Serial.print(csv_delimiter);
      Serial.print("Gain");
      Serial.print("\n");
}

void printRow(long freq, double real, double imag, double phase, double imp, double gain) {
      Serial.print(freq);
      Serial.print(csv_delimiter);
      Serial.print(real, 2);
      Serial.print(csv_delimiter);
      Serial.print(imag, 2);
      Serial.print(csv_delimiter);
      Serial.print(phase, 2);
      Serial.print(csv_delimiter);
      Serial.print(imp, 2);
      Serial.print(csv_delimiter);
      Serial.print(gain, 12);
      Serial.print("\n");
}
