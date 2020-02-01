#include <Wire.h> // I2C Arduino Library
#include "IA.h"
#include "AD5933.h"

bool IA::__debug;

/**
 * initialize the analyzer, has to bed called before calling other functions
 * @param freq - the operating freq
 * @param ref_resist -the reference resistance use for calibration
 * @return 0 - if successful, ERROR_CODE otherwise (ERROR_CODE > 0)
 */
int IA::init(unsigned long start_freq, unsigned int num_incr, unsigned long freq_incr, unsigned long ref_resist, bool debug) {
  // IMPORTANT DONT MOVE. set __debug flag here cuz DLOG depends on it
  __debug = debug;
  DLOG("IA::init()");

  Wire.begin();
  if (!AD5933::reset()) {
    DLOG("SETUP FAILED to set AD5933!");
    return ERROR_RESET;
  }
  if (!AD5933::setPowerMode(POWER_STANDBY)) {
    DLOG("SETUP FAILED to go on standby!");
    return ERROR_STANDBY;
  }
  if (!AD5933::setInternalClock(true)) {
    DLOG("SETUP FAILED to set internal clock!");
    return ERROR_ICK;
  }
  if (!AD5933::setStartFrequency(start_freq)) {
    DLOG("SETUP FAILED to set start freq!");
    return ERROR_START_FREQ;
  }
  if (!AD5933::setIncrementFrequency(freq_incr)) {
    DLOG("SETUP FAILED to set increment freq!");
    return ERROR_INC_FREQ;
  }
  if (!AD5933::setNumberIncrements(num_incr)) {
    DLOG("SETUP FAILED to set number of incremements!");
    return ERROR_NUM_INC;
  }
  if (!AD5933::setPGAGain(PGA_GAIN_X1)) {
    DLOG("SETUP FAILED to set pga gain!");
    return ERROR_PGA_GAIN;
  }
  if (!AD5933::setControlMode(CTRL_MODE_P2P_200MV)) {
    DLOG("SETUP FAILED to set P2P voltage!");
    return ERROR_SET_P2P;
  }
  if (!startSweep()) {
    DLOG("SETUP FAILED to reset sweep!");
    return ERROR_RESET_SWEEP;
  }
  return 0;
}

/**
 * 
 */
bool IA::readReadImag(int *real, int *imag) {
  DLOG("readReadImag()");
  if (!isSweeping()) {
    return false;
  }
  if (!AD5933::getComplexData(real, imag)) {
    DLOG("could not read complex data");
    return false;
  }
  return true;
}

/**
 * reset the analyzer for sweeping
 */
bool IA::startSweep() {
  DLOG("IA::resetSweep()");
  if (!AD5933::setPowerMode(POWER_STANDBY)) {
    return false;
  }
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
 * increment to the next freq for the next value reading
 */
bool IA::nextFreq() {
  AD5933::setControlMode(CTRL_INCREMENT_FREQ);
}

/**
 * repeat the current freq for the next value reading
 */
bool IA::repeatFreq() {
  AD5933::setControlMode(CTRL_REPEAT_FREQ);
}

/**
 * @return true - if the analyzer is currently sweeping, false - if not
 */
bool IA::isSweeping() {
  return (AD5933::readStatusRegister() & STATUS_SWEEP_DONE) != STATUS_SWEEP_DONE;
}

/**
 * @param real - real component of the imp
 * @param imag - imaginary component
 * @param gain - the gain factor
 * @return the impedance value
 */
double IA::calcImpedance(int real, int imag, double gain) {
  double magnitude = sqrt(pow(real, 2) + pow(imag, 2));
  return 1 / (magnitude * gain);
}

double IA::calcGain(int real, int imag, long ref_resist) {
  double mag = sqrt(pow(real, 2) + pow(imag, 2));
  return (1.0 / ref_resist) / mag;
}

void IA::DLOG(String msg) {
  if (__debug) {
    Serial.println(msg);
  }
}
