/*
 * TesteITreceiver - receives and decodes IR data, turning a LED on while receiving
 * An IR detector/demodulator must be connected to the input RECV_PIN.
 * July, 2015
 * Miguel Fonseca
 */

#include <IRremote.h>

int RECV_PIN = 11;
IRrecv irrecv(RECV_PIN);
decode_results results;

void setup()
{
  Serial.begin(9600);
  DDRD = DDRD | B00001000;
  irrecv.enableIRIn(); // Start the receiver
}

void loop() {
  if (irrecv.decode(&results)) {            
    Serial.println(results.value,DEC);
    digitalWrite(3, HIGH);
    irrecv.resume(); // Receive the next value
  }
  delay(100);
  PORTD = B00000000;
}