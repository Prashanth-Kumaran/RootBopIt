const int roostSensor1 = 5;
const int roostSensor2 = 6;
const int roostSensor3 = 7;

void setup() {
  Serial.begin(9600);
  pinMode(roostSensor1, INPUT);
  pinMode(roostSensor2, INPUT);
  pinMode(roostSensor3, INPUT);

}

void loop() {
  Serial.println("Turmoil :(");
  if(digitalRead(roostSensor1) == HIGH){
    Serial.println("Roost 1 Set");
  }

}
