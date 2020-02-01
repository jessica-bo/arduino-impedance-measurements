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
#define ERROR_SET_P2P 9

#define CTRL_MODE_P2P_2000MV  0b00000000
#define CTRL_MODE_P2P_200MV  0b00000001
#define CTRL_MODE_P2P_400MV  0b00000010
#define CTRL_MODE_P2P_100MV 0b00000011

class IA {
  public:
    static int init(unsigned long, unsigned int, unsigned long, unsigned long, bool);
    static bool readImp(double *);
    static bool readReadImag(int*, int*);
    static bool startSweep();
    static bool nextFreq();
    static bool repeatFreq();
    static double calcGain(int, int, long);
    static double calcImpedance(int, int, double);
    
  private:
    static bool __debug;
    static bool isSweeping(void);
    static void DLOG(String);
};

#endif // IA_H
