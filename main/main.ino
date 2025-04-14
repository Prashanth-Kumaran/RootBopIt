#include <Wire.h>
#include "SoftwareSerial.h"
#include "DFRobotDFPlayerMini.h"
const int startButton = 2;
const int roostLED1 = 5;
const int roostLED0 = 6;
const int roostLED2 = 7;
const int sawmill = 4;
const int sympToken = 8;

static const uint8_t PIN_MP3_TX = 10;
static const uint8_t PIN_MP3_RX = 9;
SoftwareSerial softwareSerial(PIN_MP3_RX, PIN_MP3_TX);

 
DFRobotDFPlayerMini player;

byte B_input = 0;
bool gameActive = false;
bool sawState = false;
unsigned long startTime;
unsigned long timeLimit = 3000; // 3 seconds to respond

void setup() {
  softwareSerial.begin(9600);
  delay(1000);
  pinMode(roostLED0, OUTPUT);
  pinMode(roostLED1, OUTPUT);
  pinMode(roostLED2, OUTPUT);
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
  sawState = digitalRead(sawmill);
  
  while(!gameActive){
    if(digitalRead(startButton) == HIGH){
      gameActive = true;
      player.playFolder(1,4);
      delay(2000);
  
    }
    else{
      digitalWrite(roostLED0, LOW);
      digitalWrite(roostLED1, LOW);
      digitalWrite(roostLED2, LOW);
      delay(1000);

      digitalWrite(roostLED0, HIGH);
      digitalWrite(roostLED1, HIGH);
      digitalWrite(roostLED2, HIGH);
      delay(1000);

    }
  }
  while(gameActive){
    displayScore(score);
    digitalWrite(roostLED0, LOW);
    digitalWrite(roostLED1, LOW);
    digitalWrite(roostLED2, LOW);
    delay(500);   
    startTime = millis();

    int action = random(3);

    switch(action){
      case 0:
        player.playFolder(1,1);
        success = roostInput();
        break;

      case 1:
        player.playFolder(1, 2);
        success = sawInput(sawState);

        break;

      case 2:
        player.playFolder(1, 3);
        success = martialLaw();
        break;

    }  
    if(success){
      timeLimit = 3000 - 24 * score;
      score ++;  
      sawState = digitalRead(sawmill);
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
    Wire.beginTransmission(0x20);
    Wire.write(0x13);
    Wire.endTransmission();
    Wire.requestFrom(0x20, 1);
    B_input=Wire.read();
    if(B_input == 0x00 || digitalRead(sawmill) != sawState){
      return false;
    }    
  }
  return false;
}

bool sawInput(bool sawState){
  while(millis() - startTime < timeLimit){
    if(digitalRead(sawmill) != sawState){
      return true;
    }
    Wire.beginTransmission(0x20);
    Wire.write(0x13);
    Wire.endTransmission();
    Wire.requestFrom(0x20, 1);
    B_input=Wire.read();
    if(B_input == 0x00 || digitalRead(sympToken) == LOW){
      return false;
    }
  }
  return false;
}


bool roostInput(){

  Wire.beginTransmission(0x20);
  Wire.write(0x13);
  Wire.endTransmission();
  Wire.requestFrom(0x20, 1);
  B_input=Wire.read();

  int currentRoost = 0;

  if(B_input == 0x01){
    currentRoost = 0;
  }
  else if (B_input == 0x02){
    currentRoost = 1;
  }
  else if (B_input == 0x04){
    currentRoost = 2;
  }

  int randAdd = random(2);
  randAdd++;

  randAdd += currentRoost;

  int newRoost = randAdd % 3;

  if(newRoost == 0){
    digitalWrite(roostLED0, HIGH);
    digitalWrite(roostLED1, LOW);
    digitalWrite(roostLED2, LOW);
  }
  else if(newRoost == 1){
    digitalWrite(roostLED0, LOW);
    digitalWrite(roostLED1, HIGH);
    digitalWrite(roostLED2, LOW);
  }

  else if(newRoost == 2){
    digitalWrite(roostLED0, LOW);
    digitalWrite(roostLED1, LOW);
    digitalWrite(roostLED2, HIGH);
  }

  while (millis() - startTime < timeLimit) {
    Wire.beginTransmission(0x20);
    Wire.write(0x13);
    Wire.endTransmission();
    Wire.requestFrom(0x20, 1);
    B_input=Wire.read();

    switch(newRoost){
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
    if(digitalRead(sawmill) != sawState || digitalRead(sympToken) == LOW){
      return false;
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
