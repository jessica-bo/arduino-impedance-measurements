#include "IA.h"
#include "AD5933.h"

#define ERROR_RESET 1
#define ERROR_STANDBY 2
#define ERROR_ICK 3
#define ERROR_START_FREQ 4
#define ERROR_INC_FREQ 5
#define ERROR_NUM_INC 6
#define ERROR_PGA_GAIN 7
#define ERROR_RESET_SWEEP 8

bool isSweeping(void);
double impedance(int real, int imag, double gain);

double IA::__gain;
int IA::__ref_resist;
int IA::__freq;

/**
 * initialize the analyzer, has to bed called before calling other functions
 * @param freq - the operating freq
 * @param ref_resist -the reference resistance use for calibration
 * @return 0 - if successful, ERROR_CODE otherwise (ERROR_CODE > 0)
 */
int IA::init(int freq, int ref_resist) {
  Serial.println("IA::init()");
  if (!AD5933::reset()) {
    Serial.println("SETUP FAILED to set AD5933!");
    return ERROR_RESET;
  }
  if (!AD5933::setPowerMode(POWER_STANDBY)) {
    Serial.println("SETUP FAILED to go on standby!");
    return ERROR_STANDBY;
  }
  if (!AD5933::setInternalClock(true)) {
    Serial.println("SETUP FAILED to set internal clock!");
    return ERROR_ICK;
  }
  if (!AD5933::setStartFrequency(freq)) {
    Serial.println("SETUP FAILED to set start freq!");
    return ERROR_START_FREQ;
  }
  if (!AD5933::setIncrementFrequency(1000)) {
    Serial.println("SETUP FAILED to set increment freq!");
    return ERROR_INC_FREQ;
  }
  if (!AD5933::setNumberIncrements(10)) {
    Serial.println("SETUP FAILED to set number of incremements!");
    return ERROR_NUM_INC;
  }
  if (!AD5933::setPGAGain(PGA_GAIN_X1)) {
    Serial.println("SETUP FAILED to set pga gain!");
    return ERROR_PGA_GAIN;
  }
  if (!resetSweep()) {
    Serial.println("SETUP FAILED to reset sweep!");
    return ERROR_RESET_SWEEP;
  }
  __ref_resist = ref_resist;
  __freq = freq;
  resetGain();
  return 0;
}

/**
 * read the current impedance value into the pointer
 * @param imp - the pointer the value will be stored in
 * @return true - if stored successfully, false otherwise
 */
bool IA::readImp(double *imp) {
  Serial.println("readImp()");
  int real, imag;

  if (!isSweeping()) {
    resetSweep();
  }

  if (!AD5933::getComplexData(&real, &imag)) {
    Serial.println("could not read complex data");
    return false;
  }

  AD5933::setControlMode(CTRL_REPEAT_FREQ);
  *imp = impedance(real, imag, __gain);
}

int IA::setFreq(int freq) {
   if (!AD5933::setPowerMode(POWER_STANDBY)) {
    Serial.println("SETUP FAILED to go on standby!");
    return ERROR_STANDBY;
  }
  if (!AD5933::setStartFrequency(freq)) {
    Serial.println("SETUP FAILED to set start freq!");
    return ERROR_START_FREQ;
  }
  resetSweep();
  resetGain();
}

/**
 * reset the analyzer for sweeping
 */
bool IA::resetSweep() {
  Serial.println("IA::resetSweep()");
  // init start freq
  if (!AD5933::setControlMode(CTRL_INIT_START_FREQ)) {
    return false;
  }
  // begin frequency sweep
  if (!AD5933::setControlMode(CTRL_START_FREQ_SWEEP)) {
    return false;
  }
}

/**
 * get the gain of the circuit
 */
bool IA::resetGain() {
  Serial.println("IA::getGain()");
  int real, imag;
  AD5933::getComplexData(&real, &imag);
  double magnitude = sqrt(pow(real, 2) + pow(imag, 2));
  __gain = (1.0 / __ref_resist) / magnitude;
  return true;
}

/**
 * @return true - if the analyzer is currently sweeping, false - if not
 */
bool isSweeping() {
  return (AD5933::readStatusRegister() & STATUS_SWEEP_DONE) != STATUS_SWEEP_DONE;
}

/**
 * @param real - real component of the imp
 * @param imag - imaginary component
 * @param gain - the gain factor
 * @return the impedance value
 */
double impedance(int real, int imag, double gain) {
  double magnitude = sqrt(pow(real, 2) + pow(imag, 2));
  return 1 / (magnitude * gain);
}
