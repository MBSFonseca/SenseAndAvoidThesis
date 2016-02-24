/*
  LED test
  This program is used to test the range of the position and 
  anti-collision systems' LEDs. 
  The position LEDs shall be connected to "ledpin" pin.
  The anti-collision LEDs shall be connected to "MorsePin" pin.

  created 10 July 2015
  by Miguel Fonseca
 */
 
int greenpin = 9;  //position light system pin
int redpin = 8;  //position light system pin
int whitepin = 7;  //position light system pin
int MorsePin =  10;  //anti-collision light system pin (3)
int dot = 171; //171 for NDB timing

void setup() {
  pinMode(greenpin, OUTPUT);
  pinMode(redpin, OUTPUT);
  pinMode(whitepin, OUTPUT);
//  digitalWrite(ledpin, HIGH); //turn position light ON
  pinMode(MorsePin, OUTPUT);
}

// the loop function runs over and over again forever
void loop() {
  digitalWrite(whitepin, HIGH);
  delay(2000);
  digitalWrite(whitepin, LOW);
  delay(500);
  digitalWrite(redpin, HIGH);
  delay(2000);
  digitalWrite(redpin, LOW);
  delay(500);
  digitalWrite(greenpin, HIGH);
  delay(2000);
  digitalWrite(greenpin, LOW);
  delay(500);
  digitalWrite(MorsePin, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(3*dot);              // wait for a dash
  digitalWrite(MorsePin, LOW);    // turn the LED off by making the voltage LOW
  delay(dot);              // wait for a dot
  digitalWrite(MorsePin, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(dot);              // wait for a dot
  digitalWrite(MorsePin, LOW);    // turn the LED off by making the voltage LOW
  delay(dot);              // wait for a dot
  digitalWrite(MorsePin, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(dot);              // wait for a dot
  digitalWrite(MorsePin, LOW);    // turn the LED off by making the voltage LOW
  //delay(7*dot);              // wait for a next word
  digitalWrite(MorsePin, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(3*dot);              // wait for a dash
  digitalWrite(MorsePin, LOW);    // turn the LED off by making the voltage LOW
  delay(dot);              // wait for a dot
  digitalWrite(MorsePin, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(dot);              // wait for a dot
  digitalWrite(MorsePin, LOW);    // turn the LED off by making the voltage LOW
  delay(dot);              // wait for a dot
  digitalWrite(MorsePin, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(dot);              // wait for a dot
  digitalWrite(MorsePin, LOW);    // turn the LED off by making the voltage LOW
  delay(500);
  delay(2000);
}
