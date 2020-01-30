#include "IA.h"
#include "AD5933.h"

double IA::__gain;
unsigned long IA::__ref_resist;
unsigned long IA::__freq;
bool IA::__debug;

/**
 * initialize the analyzer, has to bed called before calling other functions
 * @param freq - the operating freq
 * @param ref_resist -the reference resistance use for calibration
 * @return 0 - if successful, ERROR_CODE otherwise (ERROR_CODE > 0)
 */
int IA::init(unsigned long freq, unsigned long ref_resist, bool debug) {
  // IMPORTANT DONT MOVE. set __debug flag here cuz DLOG depends on it
  __debug = debug;
  DLOG("IA::init()");
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
  if (!AD5933::setStartFrequency(freq)) {
    DLOG("SETUP FAILED to set start freq!");
    return ERROR_START_FREQ;
  }
  if (!AD5933::setIncrementFrequency(1000)) {
    DLOG("SETUP FAILED to set increment freq!");
    return ERROR_INC_FREQ;
  }
  if (!AD5933::setNumberIncrements(10)) {
    DLOG("SETUP FAILED to set number of incremements!");
    return ERROR_NUM_INC;
  }
  if (!AD5933::setPGAGain(PGA_GAIN_X1)) {
    DLOG("SETUP FAILED to set pga gain!");
    return ERROR_PGA_GAIN;
  }
  if (!resetSweep()) {
    DLOG("SETUP FAILED to reset sweep!");
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
  DLOG("readImp()");
  int real, imag;

  if (!isSweeping()) {
    resetSweep();
  }

  if (!AD5933::getComplexData(&real, &imag)) {
    DLOG("could not read complex data");
    return false;
  }

  AD5933::setControlMode(CTRL_REPEAT_FREQ);
  *imp = impedance(real, imag, __gain);
}

bool IA::readReadImag(int *real, int *imag) {
  DLOG("readReadImag()");
  if (!isSweeping()) {
    resetSweep();
  }

  if (!AD5933::getComplexData(real, imag)) {
    DLOG("could not read complex data");
    return false;
  }
  
  AD5933::setControlMode(CTRL_REPEAT_FREQ);
  return true;
}

/**
 * change the frequency of the IA
 * @param freq - frequency to change to
 */
int IA::setFreq(int freq) {
   if (!AD5933::setPowerMode(POWER_STANDBY)) {
    DLOG("SETUP FAILED to go on standby!");
    return ERROR_STANDBY;
  }
  if (!AD5933::setStartFrequency(freq)) {
    DLOG("SETUP FAILED to set start freq!");
    return ERROR_START_FREQ;
  }
  resetSweep();
  resetGain();
}

/**
 * reset the analyzer for sweeping
 */
bool IA::resetSweep() {
  DLOG("IA::resetSweep()");
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
  // TODO: get multiple points then take the average to increase accuracy
  DLOG("IA::resetGain()");
  
  int n = 5;
  int real, imag;
  double totalMag = 0;
  
  for (int i = 0; i < n; i++) {
//    DLOG("waiting to read real and imag data");
    AD5933::getComplexData(&real, &imag);
    totalMag += sqrt(pow(real, 2) + pow(imag, 2));
//    DLOG("Set control mode to repeat");
    AD5933::setControlMode(CTRL_REPEAT_FREQ);
  }
  
  double avgMag = totalMag / n;
  __gain = (1.0 / __ref_resist) / avgMag;
  
//  Serial.println(__gain);
  return true;
}

/**
 * @return true - if the analyzer is currently sweeping, false - if not
 */
bool IA::isSweeping() {
  return (AD5933::readStatusRegister() & STATUS_SWEEP_DONE) != STATUS_SWEEP_DONE;
}

double IA::getGain() {
  DLOG("IA::resetGain()");
  resetGain();
  return __gain;
}

void IA::setGain(double gain) {
  DLOG("IA::setGain()");
  
  __gain = gain;
}

/**
 * @param real - real component of the imp
 * @param imag - imaginary component
 * @param gain - the gain factor
 * @return the impedance value
 */
double IA::impedance(int real, int imag, double gain) {
  double magnitude = sqrt(pow(real, 2) + pow(imag, 2));
  return 1 / (magnitude * gain);
}

void IA::DLOG(String msg) {
  if (__debug) {
    Serial.println(msg);
  }
}
