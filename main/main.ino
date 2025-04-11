#include <Wire.h>
const int roostLED1 = 5;
const int roostLED2 = 6;
const int roostLED3 = 7;
const int sawmill = 4; 
//const int sawLED = 10;
const int photores = 8;
//const int photoLED = 9;
const int startButton = 2;

byte B_input = 0;
bool gameActive = false;
unsigned long startTime;
const int timeLimit = 3000; // 3 seconds to respond

void setup() {
  pinMode(roostLED1, OUTPUT);
  pinMode(roostLED2, OUTPUT);
  pinMode(roostLED3, OUTPUT);
  pinMode(sawmill, INPUT);



  Wire.begin(); // wake up I2C bus
  Wire.beginTransmission(0x20);
  Wire.write(0x00); // IODIRA register
  Wire.write(0x00); // set entire PORT A as output
  Wire.endTransmission();
}

void loop() {
  
  if(!gameActive){
    if(digitalRead(startButton) == HIGH){
      gameActive = true;
      delay(1000);
      gameLoop();
    }
  }

}

void gameLoop(){
  int score = 0;
  while(gameActive){
    displayScore(score);
    int action = random(5); //0-2: roosts, 3: sawmill, 4: mouse
    startTime = millis();
    bool success = false;
    switch(action){
      case 0:
        digitalWrite(roostLED1, HIGH);
        success = roostinput(1);
      case 1:
        digitalWrite(roostLED2, HIGH);
        success = roostinput(2);
      case 2: 
        digitalWrite(roostLED3, HIGH);
        success = roostinput(3);
      case 3:
        success = sawinput();
      case 4:
        success = mouseinput();
    }

    if(success){
      score++; 
    }
    else{
      gameActive = false;
    }
  }
}

bool roostinput(int r){
  while (millis() - startTime < timeLimit) {
    Wire.beginTransmission(0x20);
    Wire.write(0x13);
    Wire.endTransmission();
    Wire.requestFrom(0x20, 1);
    B_input=Wire.read();

    if (B_input == pow(r, 2)) {
      return true;
    }
  }
  return false;
}
  

bool sawinput(){
  while(millis() - startTime < timeLimit){
    if(sawmill){
      return true;
    }
  }
  return false;
}

bool mouseinput(){
  while(millis() - startTime < timeLimit){
    if(photores){
      return true;
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

