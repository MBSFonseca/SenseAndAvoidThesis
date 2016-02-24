/*
 * TesteITsender - periodically sends '0', '1' and '2' using RC-5 protocol
 * July, 2015
 * Miguel Fonseca
 */

#include <IRremote.h>
IRsend irsend;

void setup()
{
  Serial.begin(9600);
}

void loop() {
  for (int i = 0; i < 3; i++) {
    irsend.sendRC5(i, 6);
    delay(114);
  }
  delay(500);
}
