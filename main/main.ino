//Headers
#include "Wire.h"

//Pin number declerations
const int startButton = 2;

const int roostSensor1 = 5;
const int roostSensor2 = 6;
const int roostSensor3 = 7;



void setup() {



  //Setting Pinmodes
  pinMode(startButton, INPUT);
  pinMode(roostSensor1, INPUT);
  pinMode(roostSensor2, INPUT);
  pinMode(roostSensor3, INPUT);

  //I2C Extender Setup
  Wire.begin(); // wake up I2C bus
  // set I/O pins to outputs
  Wire.beginTransmission(0x20);
  Wire.write(0x00); // IODIRA register
  Wire.write(0x00); // set all of port A to outputs
  Wire.endTransmission();
  Wire.beginTransmission(0x20);
  Wire.write(0x01); // IODIRB register
  Wire.write(0x00); // set all of port B to outputs
  Wire.endTransmission();

}

void loop() {
  int gameState = 0;
  int score = 0;
  unsigned long inputTimeout = 2000;
  startWait(); // waits for start button to be pressed
  gameState = 1;
  while(gameState == 1){
    bool inputSuccess = false;
    inputSuccess = birdAction(inputTimeout);
    if(inputSuccess == false){
      gameState = 2;
    }
  }
  

}

void startWait(){
  while(true){
    if(digitalRead(startButton) == HIGH){
      break;
    }
  }
}

bool birdAction(unsigned long inputTimeout){
  int newPosition = random(0, 3);
  byte birdLight = pow(2, newPosition);
  Wire.beginTransmission(0x20);
  Wire.write(0x13); // GPIOB
  Wire.write(birdLight); // port B
  Wire.endTransmission();
  
  unsigned long countDownStart = millis();
  while(millis() - countDownStart < inputTimeout){
    if(digitalRead(newPosition + 5 == HIGH)){
      return true;
    }
  }
  return false;
}