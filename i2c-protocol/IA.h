#ifndef IA_H
#define IA_H

#include <Arduino.h>
#include <Wire.h>


#define ERROR_RESET 1
#define ERROR_STANDBY 2
#define ERROR_ICK 3
#define ERROR_START_FREQ 4
#define ERROR_INC_FREQ 5
#define ERROR_NUM_INC 6
#define ERROR_PGA_GAIN 7
#define ERROR_RESET_SWEEP 8

class IA {
  public:
    static int init(int, int);
    static bool readImp(double *);
    static int setFreq(int);

  private:
    static double __gain;
    static int __ref_resist;
    static int __freq;
    static bool resetSweep();
    static bool resetGain();
    static bool isSweeping(void);
    static double impedance(int, int, double);
};

#endif // IA_H
