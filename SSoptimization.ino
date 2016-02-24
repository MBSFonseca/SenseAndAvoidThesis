/*
 * SSoptimization - optimize TX, A, REC and RC5_T1
 * Waits for variables given by PC and then functions as Sense and Avoid Prototype
 * Used to test combinations of given variables
 * January, 2016
 * Miguel Fonseca
 */

#include <IRremote.h>
#include <morseaircraft.h>

byte segment[] = {B00000000, B00000100, B00001000, B00001100, B00010000, B00010100, B00011000, B00011100, B00100000, B00100100, B00101000, B00101100, B00110000, B00110100, B00111000, B00111100};
int RECV_PIN[NRECV] = {8, 10, 11, 12};
IRsend irsend;
IRrecv irrecv(RECV_PIN);
decode_results results[NRECV];
int iteracao = 1;
int TX;
int A;
int REC;
int rando;


// Anti-collision LED related stuff
MorseAircraft morse("UH");

boolean arrayIncludeElement(String array[], String element, int tamanho) {
  for (int i = 0; i < tamanho; i++) {
    if (array[i] == element) {
      return true;
    }
  }
  return false;
}

int recv(int variables[], int tamanho) {
  bool test = 0;
  int a;
  //static char a[16];
  Serial.flush();
  while (!test) {
    if (Serial.available() > 0) {
      char c = Serial.read();
      a = (int)c -48;
    }
    if (a >0 && a<=9) {
      return (variables[a-1]);
    }
  }
}

void setup()
{
  DDRD = DDRD | B00111100; // set digital pins 2,3,4,5 to outputs
  DDRB = DDRB | B00100010; // set digital pin 9 and 13 (collisionled) to outputs
  Serial.begin(115200);
  Serial.flush();
  delay(10);
  int variables[] = {1, 5, 10, 15, 20};
  TX = recv(variables, 5); //receive TX from PC
  Serial.flush();
  delay(10);
  int variable[] = {1, 3, 5, 10};
  A = recv(variable, 4);  //receive A from PC
  Serial.flush();
  delay(10);
  int variabl[] = {10, 20, 40, 60, 100};
  REC = recv(variabl, 5);  //receive REC from PC
  String t = String("TX" + String(TX) + ", A" + String(A) + ", REC" + String(REC)); //confirm received variables
  Serial.println(t);
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
      morse.flash();
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
