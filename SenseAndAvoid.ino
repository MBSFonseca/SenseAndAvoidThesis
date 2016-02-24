
/*
 * SenseAndAvoid - sense and avoid prototype
 * 16 IR LEDs identify a sector each and the type of aircraft, 4 receivers cover 360 degrees around the aircraft
 * Adding to this, an anti-collision LED may be connected to port 13 to blink the aircraft's type in morse code
 * January, 2016
 * Miguel Fonseca
 */

#include <IRremote.h>
#include <morseaircraft.h>

#define TX 10
#define A 3
#define REC 15

//Bytes with PIN's output state to control multiplexer
byte segment[] = {B00000000, B00000100, B00001000, B00001100, B00010000, B00010100, B00011000, B00011100, B00100000, B00100100, B00101000, B00101100, B00110000, B00110100, B00111000, B00111100};
//Receiver PINs - Edit NRECV in IRremote.h
int RECV_PIN[NRECV] = {8, 10, 11, 12};
IRsend irsend;
IRrecv irrecv(RECV_PIN);
decode_results results[NRECV];
int iteracao = 1;
int rando;

// Anti-collision LED related stuff
MorseAircraft morse("UH");

void setup()
{
  DDRD = DDRD | B00111100; // set digital pins 2,3,4,5 to outputs
  DDRB = DDRB | B00100010; // set digital pin 9 and 13 (collisionled) to outputs
  Serial.begin(115200);
  Serial.flush();
  irrecv.enableIRIn(); // Start the receiver
  TIMSK1 = 0; // Disable Timer1 Overflow Interrupt
  //Configure PWM
  const uint16_t pwmval = 16000000 / 2000 / 56;
  TCCR1A = _BV(WGM11); \
  TCCR1B = _BV(WGM13) | _BV(CS10); \
  ICR1 = pwmval; \
  OCR1A = pwmval / 2; \

  rando = random(1, TX);
}

void loop() {
  String dataString;
  morse.flash();
  if (iteracao == rando) {
    for (int led = 0; led < 16; led++) {
      PORTD = segment[led];      // select LED
      irsend.sendRC5(led, 4, morse.number);
      delay(A);
    }
    PORTD = segment[0];
  }
  delay(REC);

  irrecv.decode(results);
  for (int b = 0; b < NRECV; b++) {
    if (results[b].decoder == 1) {
      if (dataString[0] != '\0') dataString += ",";
      dataString += String(b);
      dataString += ",";
      dataString += String(results[b].value, DEC);
      dataString += ",";
      dataString += String(results[b].type, DEC);
      irrecv.resume(b); // Receive the next value
    }
  }
  if (dataString[0] != '\0') Serial.println(dataString);

  iteracao = iteracao + 1;
  if (iteracao == TX + 1) {
    iteracao = 1;
    rando = random(1, TX);
  }
}
