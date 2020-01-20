#ifndef IA_H
#define IA_H

#include <Arduino.h>
#include <Wire.h>

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
};

#endif // IA_H
