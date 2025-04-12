#include "SoftwareSerial.h"
#include "DFRobotDFPlayerMini.h"
#include <Wire.h>

static const uint8_t PIN_MP3_TX = 10;
static const uint8_t PIN_MP3_RX = 9;
SoftwareSerial softwareSerial(PIN_MP3_RX, PIN_MP3_TX);
int LED1 = 8;
 
DFRobotDFPlayerMini player;

void setup () {
  softwareSerial.begin(9600);
  delay(1000);
  
  pinMode(LED1, OUTPUT);
  if(player.begin(softwareSerial))
  {
    player.volume(8);
    player.playFolder(1,1);
    delay(10000);
    player.playFolder(1,2);
    delay(10000);
    player.playFolder(1,3);
  }
  else
  {
    digitalWrite(LED1, HIGH);
    while(true);
  }
  
}


void loop () {

}
