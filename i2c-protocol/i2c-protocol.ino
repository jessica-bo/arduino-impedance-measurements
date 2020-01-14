#include <AD5933.h>
#include <Wire.h> //I2C Arduino Library

#define address 0x0D //I2C 7bit address
#define bytes 6

void setup(){
  //SDA and SLA pins are A4 and A5 on the Arduino Uno
  //Initialize Serial and I2C communications
  Wire.begin();
  Serial.begin(9600);
  
  Wire.beginTransmission(address);
  Wire.write(0x80);
  Wire.write(0x17);
  Wire.endTransmission();
}

void loop(){
   
  int val; 

  Wire.beginTransmission(address);
  Wire.write(0x80); /??
  Wire.endTransmission();

//  Wire.requestFrom(address, bytes);
//
//  if (bytes <= Wire.available()) {
//    val = Wire.read();
//    Serial.println(val);
 
  delay(100);

}
