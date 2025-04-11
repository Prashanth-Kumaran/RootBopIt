#include <Wire.h>
const int startButton = 4;
const int roostLED1 = 5;
const int roostLED2 = 6;
const int roostLED3 = 7;

byte B_input = 0;
bool gameActive = false;
unsigned long startTime;
const int timeLimit = 3000; // 3 seconds to respond

void setup() {
  pinMode(roostLED1, OUTPUT);
  pinMode(roostLED2, OUTPUT);
  pinMode(roostLED3, OUTPUT);

  Wire.begin(); // wake up I2C bus
  Wire.beginTransmission(0x20);
  Wire.write(0x00); // IODIRA register
  Wire.write(0x00); // set entire PORT A as output
  Wire.endTransmission();
}

void loop() {

  Wire.beginTransmission(0x20);
  Wire.write(0x13);
  Wire.endTransmission();
  Wire.requestFrom(0x20, 1);
  B_input=Wire.read();
  for(int i = 0; i < 100; i++){
    byte hexout = decimalToHex(i);
    Wire.beginTransmission(0x20);
    Wire.write(0x12); // IODIRA register
    Wire.write(hexout); // set entire PORT A as output
    Wire.endTransmission();
    delay(10);

  }

  

  if(B_input & 0x01){
    digitalWrite(roostLED1, HIGH);
  }
  else if(B_input & 0x02){
    digitalWrite(roostLED2, HIGH);
  }
  else if(B_input & 0x04){
    digitalWrite(roostLED3, HIGH);
  }
  else{
    digitalWrite(roostLED1, LOW);
    digitalWrite(roostLED2, LOW);
    digitalWrite(roostLED3, LOW);
  }


}

byte decimalToHex(int num) {
  if (num < 0 || num > 99) return 0xFF; // only two-digit decimals

  int tens = num / 10;
  int ones = num % 10;

  if (tens < 16 && ones < 16) {
    return (tens << 4) | ones; // combine into one byte: 0xTTOO
  }

  return 0xFF; // error code
}

