/*
 * morseaircraft - Library to generate Morse code that identifies an aircraft's category.
 * Created by Miguel Saraiva Fonseca, December, 2015.
 * Released into the public domain.
 */

#ifndef morseaircraft_h
#define morseaircraft_h

#include "Arduino.h"
#define COLLISIONLED 13
class MorseAircraft
{
  public:
    MorseAircraft(char type[]);
    void flash();
	int number;
  private:
    unsigned long message;
	unsigned long previousMillis;
	long interval;
	int order;
	long temp;
};

#endif