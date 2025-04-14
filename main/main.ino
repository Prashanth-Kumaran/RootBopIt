#include <Wire.h>
#include "SoftwareSerial.h"
#include "DFRobotDFPlayerMini.h"
const int startButton = 2;
const int roostLED2 = 5;
const int roostLED1 = 6;
const int roostLED3 = 7;
const int sawmill = 4;
const int sympToken = 8;

static const uint8_t PIN_MP3_TX = 10;
static const uint8_t PIN_MP3_RX = 9;
SoftwareSerial softwareSerial(PIN_MP3_RX, PIN_MP3_TX);

 
DFRobotDFPlayerMini player;

byte B_input = 0;
bool gameActive = false;
unsigned long startTime;
unsigned long timeLimit = 3000; // 3 seconds to respond

void setup() {
  softwareSerial.begin(9600);
  delay(1000);
  pinMode(roostLED1, OUTPUT);
  pinMode(roostLED2, OUTPUT);
  pinMode(roostLED3, OUTPUT);
  player.begin(softwareSerial);
  player.volume(30);

  Wire.begin(); // wake up I2C bus
  Wire.beginTransmission(0x20);
  Wire.write(0x00); // IODIRA register
  Wire.write(0x00); // set entire PORT A as output
  Wire.endTransmission();
}


void loop() {
  int score = 0;
  timeLimit = 3000;
  bool success = false;
  bool sawState = digitalRead(sawmill);
  
  while(!gameActive){
    if(digitalRead(startButton) == HIGH){
      gameActive = true;
      player.playFolder(1,4);
      delay(2000);
  
    }
    else{
      digitalWrite(roostLED1, LOW);
      digitalWrite(roostLED2, LOW);
      digitalWrite(roostLED3, LOW);
      delay(1000);

      digitalWrite(roostLED1, HIGH);
      digitalWrite(roostLED2, HIGH);
      digitalWrite(roostLED3, HIGH);
      delay(1000);

    }
  }
  int oldaction = -1;
  int action = 0;
  while(gameActive){
    displayScore(action);
    digitalWrite(roostLED1, LOW);
    digitalWrite(roostLED2, LOW);
    digitalWrite(roostLED3, LOW);
    delay(500);   
    startTime = millis();
    do{
      action = random(5);
    }while(action == oldaction);

    switch(action){
      case 0:
        player.playFolder(1,1);
        digitalWrite(roostLED1, HIGH);
        digitalWrite(roostLED2, LOW);
        digitalWrite(roostLED3, LOW);

        delay(300);
        success = roostInput(0);

        break;
      case 1:
        player.playFolder(1,1);
        digitalWrite(roostLED2, HIGH);
        digitalWrite(roostLED1, LOW);
        digitalWrite(roostLED3, LOW);
        delay(300);
        success = roostInput(1);

        break;
      case 2: 
        player.playFolder(1,1);

        digitalWrite(roostLED3, HIGH);
        digitalWrite(roostLED2, LOW);
        digitalWrite(roostLED1, LOW);

        delay(300);
        success = roostInput(2);
        break;

      case 3:
        player.playFolder(1, 2);
        success = sawInput(sawState);

        break;

      case 4:
        player.playFolder(1, 3);
        success = martialLaw();
        break;

    }  
    if(success){
      timeLimit = 3000 - 24 * score;
      score ++;  
      sawState = digitalRead(sawmill);
      oldaction = action;
    }
    else{
      gameActive = false;
    }

  }

  


}

bool martialLaw(){
  while(millis() - startTime < timeLimit){
    if(digitalRead(sympToken) == LOW){
      return true;
    }
  }
  return false;
}

bool sawInput(bool sawState){
  while(millis() - startTime < timeLimit){
    if(digitalRead(sawmill) != sawState){
      return true;
    }
  }
  return false;
}
bool roostInput(int r){
  while (millis() - startTime < timeLimit) {
    Wire.beginTransmission(0x20);
    Wire.write(0x13);
    Wire.endTransmission();
    Wire.requestFrom(0x20, 1);
    B_input=Wire.read();

    switch(r){
      case 0:
        if(B_input == 0x01){
          return true;
        }
        break;
      case 1:
        if(B_input == 0x02){
          return true;
        }
        break;
      case 2:
        if(B_input == 0x04){
          return true;
        }
      break;
    }
  }
  return false;
}


void displayScore(int num) {
  if (num < 0 || num > 99) return; // only two-digit decimals

  byte hexout = 0;
  int tens = num / 10;
  int ones = num % 10;

  if (tens < 16 && ones < 16) {
     hexout = (tens << 4) | ones; // combine into one byte: 0xTTOO
  }

  Wire.beginTransmission(0x20);
  Wire.write(0x12); // IODIRA register
  Wire.write(hexout); // set entire PORT A as output
  Wire.endTransmission();
  delay(10);  
}
