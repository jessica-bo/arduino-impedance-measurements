#include <Wire.h> // I2C Arduino Library
#include "AD5933.h" // C++ library for AD5933 impedance analyzer chip

#define address 0x0D //I2C 7bit address

#define START_FREQ  (30000)
#define FREQ_INCR   (0)//(1000)
#define NUM_INCR    (50)
#define REF_RESIST  (10000)

double gain[NUM_INCR+1];
int phase[NUM_INCR+1];

void setup(){
  
  // Begin I2C
  Wire.begin();

  // Begin serial at 9600 baud for output
  Serial.begin(9600);
  Serial.println("AD5933 Test Started!");

  // Perform initial configuration. Fail if any one of these fail
  
  if (!(AD5933::reset() &&
        AD5933::setPowerMode(POWER_STANDBY) &&
        AD5933::setInternalClock(true) &&
        AD5933::setStartFrequency(START_FREQ) &&
        AD5933::setIncrementFrequency(FREQ_INCR) &&
        AD5933::setNumberIncrements(NUM_INCR) &&
        AD5933::setPGAGain(PGA_GAIN_X1))) {
            Serial.println("FAILED in initialization!");
            while (true) ;
        }
   

  // Perform calibration sweep
  if (AD5933::calibrate(gain, phase, REF_RESIST, NUM_INCR+1))
    Serial.println("Calibrated!");
  else
    Serial.println("Calibration failed...");

  
//  //SDA and SLA pins are A4 and A5 on the Arduino Uno
//  //Initialize Serial and I2C communications
//  Serial.begin(9600);
//  Serial.print("Initialized");
//  
//  Wire.begin();
//  Wire.beginTransmission(address);
//  Wire.write(0x80);
//  Wire.write(0x17);
//  Wire.endTransmission();
//  Serial.print("I2C set up");
}

void loop(){
  frequencySweepEasy();
  //delay(1000);
  //delay(10);
}

void frequencySweepEasy() {
    // Create arrays to hold the data
    int real[NUM_INCR+1], imag[NUM_INCR+1];

    // Perform the frequency sweep
    if (AD5933::frequencySweep(real, imag, NUM_INCR+1)) {
      // Print the frequency data
      double cfreq = START_FREQ/1000;
      for (int i = 0; i < NUM_INCR+1; i++, cfreq += FREQ_INCR/1000) {
        // Print raw frequency data
       // Serial.print(cfreq);
//        Serial.print(": R=");
//        Serial.print(real[i]);
//        Serial.print("/I=");
//        Serial.print(imag[i]);

        // Compute impedance
        double magnitude = sqrt(pow(real[i], 2) + pow(imag[i], 2));
        double impedance = 1/(magnitude*gain[i]);
//        Serial.print("  |Z|=");
        Serial.println(impedance);
//        Serial.print("  gain=");
//        Serial.println(gain[i]);
      }
//      Serial.println("Frequency sweep complete!");
    } else {
//      Serial.println("Frequency sweep failed...");
    }
}


