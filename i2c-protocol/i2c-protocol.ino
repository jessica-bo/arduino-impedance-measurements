//#include <AD5933.h>
#include <Wire.h> //I2C Arduino Library
#include "AD5933.h"

#define address 0x0D //I2C 7bit address
#define bytes 6

#define START_FREQ  (80000)
#define FREQ_INCR   (1000)
#define NUM_INCR    (40)
#define REF_RESIST  (10000)

double gain[NUM_INCR+1];
int phase[NUM_INCR+1];

void setup(){
  
  // Begin I2C
  Wire.begin();

  // Begin serial at 9600 baud for output
  Serial.begin(9600);
  Serial.println("AD5933 Test Started!");

  // Perform initial configuration. Fail if any one of these fail.

  if (AD5933::reset()) {
    Serial.print("reset successful");
  } else {
    Serial.print("reset failed");
  }
  
  if (!(AD5933::reset() &&
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
   
  int val; 

  Wire.beginTransmission(address);
//  Wire.write(0x80); //??
  Wire.endTransmission();
  Serial.print(5);

//  Wire.requestFrom(address, bytes);
//
//  if (bytes <= Wire.available()) {
//    val = Wire.read();
//    Serial.println(val);
 
  delay(100);

}
