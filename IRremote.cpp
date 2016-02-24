/*
 * IRremote
 * Version 0.11 August, 2009
 * Copyright 2009 Ken Shirriff
 * For details, see http://arcfn.com/2009/08/multi-protocol-infrared-remote-library.html
 *
 * Modified by Miguel Fonseca for sense and avoid prototype - January 2016
 */

#include "IRremote.h"
#include "IRremoteInt.h"

// Provides ISR
#include <avr/interrupt.h>

volatile irparams_t irparams[NRECV];

// These versions of MATCH, MATCH_MARK, and MATCH_SPACE are only for debugging.
// To use them, set DEBUG in IRremoteInt.h
// Normally macros are used for efficiency
#ifdef DEBUG
int MATCH(int measured, int desired) {
  Serial.print("Testing: ");
  Serial.print(TICKS_LOW(desired), DEC);
  Serial.print(" <= ");
  Serial.print(measured, DEC);
  Serial.print(" <= ");
  Serial.println(TICKS_HIGH(desired), DEC);
  return measured >= TICKS_LOW(desired) && measured <= TICKS_HIGH(desired);
}

int MATCH_MARK(int measured_ticks, int desired_us) {
  Serial.print("Testing mark ");
  Serial.print(measured_ticks * USECPERTICK, DEC);
  Serial.print(" vs ");
  Serial.print(desired_us, DEC);
  Serial.print(": ");
  Serial.print(TICKS_LOW(desired_us + MARK_EXCESS), DEC);
  Serial.print(" <= ");
  Serial.print(measured_ticks, DEC);
  Serial.print(" <= ");
  Serial.println(TICKS_HIGH(desired_us + MARK_EXCESS), DEC);
  return measured_ticks >= TICKS_LOW(desired_us + MARK_EXCESS) && measured_ticks <= TICKS_HIGH(desired_us + MARK_EXCESS);
}

int MATCH_SPACE(int measured_ticks, int desired_us) {
  Serial.print("Testing space ");
  Serial.print(measured_ticks * USECPERTICK, DEC);
  Serial.print(" vs ");
  Serial.print(desired_us, DEC);
  Serial.print(": ");
  Serial.print(TICKS_LOW(desired_us - MARK_EXCESS), DEC);
  Serial.print(" <= ");
  Serial.print(measured_ticks, DEC);
  Serial.print(" <= ");
  Serial.println(TICKS_HIGH(desired_us - MARK_EXCESS), DEC);
  return measured_ticks >= TICKS_LOW(desired_us - MARK_EXCESS) && measured_ticks <= TICKS_HIGH(desired_us - MARK_EXCESS);
}
#else
int MATCH(int measured, int desired) {return measured >= TICKS_LOW(desired) && measured <= TICKS_HIGH(desired);}
int MATCH_MARK(int measured_ticks, int desired_us) {return MATCH(measured_ticks, (desired_us + MARK_EXCESS));}
int MATCH_SPACE(int measured_ticks, int desired_us) {return MATCH(measured_ticks, (desired_us - MARK_EXCESS));}
// Debugging versions are in IRremote.cpp
#endif

// Note: first bit must be a one (start bit)
//void IRsend::sendRC5(unsigned long data, int nbits){
void IRsend::sendRC5(unsigned long data, int nbits, unsigned long temp){
  PORTB &= ~(_BV(PORTB1)); //digitalWrite(TIMER_PWM_PIN, LOW); // When not sending PWM, we want it low
  temp = temp << (32 - 5);  
  data = data << (32 - nbits);
  // Disable the Timer2 Interrupt (which is used for receiving IR)
  TIMER_DISABLE_INTR;
  mark(RC5_T1); // First start bit
  space(RC5_T1); // Second start bit
  mark(RC5_T1); // Second start bit
  mark(RC5_T1); //Toggle bit 
  space(RC5_T1); //Toggle bit
  
  //5 bit for aircraft type
  for (int i = 0; i < 5; i++) {
    if (temp & TOPBIT) {
      space(RC5_T1); // 1 is space, then mark
      mark(RC5_T1);
    } 
    else {
      mark(RC5_T1);
      space(RC5_T1);
    }
    temp <<= 1;
  }
  
  for (int i = 0; i < nbits; i++) {
    if (data & TOPBIT) {
      space(RC5_T1); // 1 is space, then mark
      mark(RC5_T1);
    } 
    else {
      mark(RC5_T1);
      space(RC5_T1);
    }
    data <<= 1;
  }
  space(0); // Turn off at end
  TIMER_ENABLE_INTR;
}

void IRsend::mark(int time) {
  // Sends an IR mark for the specified number of microseconds.
  // The mark output is modulated at the PWM frequency.
  TIMER_ENABLE_PWM; // Enable pin 3 PWM output
  if (time > 0) delayMicroseconds(time);
}
/* Leave pin off for time (given in microseconds) */
void IRsend::space(int time) {
  // Sends an IR space for the specified number of microseconds.
  // A space is no output, so the PWM output is disabled.
  TIMER_DISABLE_PWM; // Disable pin 3 PWM output
  if (time > 0) delayMicroseconds(time);
}
/*void IRsend::enableIROut(int khz) {
  // Enables IR output.  The khz value controls the modulation frequency in kilohertz.
  // The IR output will be on pin 3 (OC2B).
  // This routine is designed for 36-40KHz; if you use it for other values, it's up to you
  // to make sure it gives reasonable results.  (Watch out for overflow / underflow / rounding.)
  // TIMER2 is used in phase-correct PWM mode, with OCR2A controlling the frequency and OCR2B
  // controlling the duty cycle.
  // There is no prescaling, so the output frequency is 16MHz / (2 * OCR2A)
  // To turn the output on and off, we leave the PWM running, but connect and disconnect the output pin.
  // A few hours staring at the ATmega documentation and this will all make sense.
  // See my Secrets of Arduino PWM at http://arcfn.com/2009/07/secrets-of-arduino-pwm.html for details.

  // timer2
  // COM2A = 00: disconnect OC2A
  // COM2B = 00: disconnect OC2B; to send signal set to 10: OC2B non-inverted
  // WGM2 = 101: phase-correct PWM with OCRA as top
  // CS2 = 001: no prescaling
  // The top value for the timer.  The modulation frequency will be SYSCLOCK / 2 / OCR2A.
  TIMER_CONFIG_KHZ(khz);
}*/
IRrecv::IRrecv(int *recvpin){
  for(int a = 0; a < NRECV; a++) {
	irparams[a].recvpin = recvpin[a];
  }
}
// initialization
void IRrecv::enableIRIn() {
  unsigned char sreg;
  sreg = SREG;  // save interrupts state
  cli();
  // setup pulse clock timer interrupt
  //Prescale /8 (16M/8 = 0.5 microseconds per tick)
  // Therefore, the timer interval can range from 0.5 to 128 microseconds
  // depending on the reset value (255 to 0)
  TIMER_CONFIG_NORMAL();

  //Timer2 Overflow Interrupt Enable
  TIMER_ENABLE_INTR;

  TIMER_RESET;

  SREG = sreg;  // reset interrupts state
  for(int a = 0; a < NRECV; a++) {
	  // initialize state machine variables
	  irparams[a].rcvstate = STATE_IDLE;
	  irparams[a].rawlen = 0;

	  // set pin modes
	  pinMode(irparams[a].recvpin, INPUT);
  }
}

// TIMER2 interrupt code to collect raw data.
// Widths of alternating SPACE, MARK are recorded in rawbuf.
// Recorded in ticks of 50 microseconds.
// rawlen counts the number of entries recorded so far.
// First entry is the SPACE between transmissions.
// As soon as a SPACE gets long, ready is set, state switches to IDLE, timing of SPACE continues.
// As soon as first MARK arrives, gap width is recorded, ready is cleared, and new logging starts
ISR(TIMER_INTR_NAME){
  TIMER_RESET;
  uint8_t irdata[NRECV];
  for(int a = 0; a < NRECV; a++) {
	irdata[a] = (uint8_t)digitalRead(irparams[a].recvpin);
	irparams[a].timer++;
	if (irparams[a].rawlen >= RAWBUF) {
    // Buffer overflow
    irparams[a].rcvstate = STATE_STOP;
    }
	switch(irparams[a].rcvstate) {
		case STATE_IDLE: // In the middle of a gap
			if (irdata[a] == MARK) {
				if (irparams[a].timer < GAP_TICKS) {
					// Not big enough to be a gap.
					irparams[a].timer = 0;
				} 
				else {
					// gap just ended, record duration and start recording transmission
					irparams[a].rawlen = 0;
					irparams[a].rawbuf[irparams[a].rawlen++] = irparams[a].timer;
					irparams[a].timer = 0;
					irparams[a].rcvstate = STATE_MARK;
					}
				}
			break;
	    case STATE_MARK: // timing MARK
			if (irdata[a] == SPACE) {   // MARK ended, record time
			irparams[a].rawbuf[irparams[a].rawlen++] = irparams[a].timer;
			irparams[a].timer = 0;
			irparams[a].rcvstate = STATE_SPACE;
			}
			break;
		case STATE_SPACE: // timing SPACE
			if (irdata[a] == MARK) { // SPACE just ended, record it
			  irparams[a].rawbuf[irparams[a].rawlen++] = irparams[a].timer;
			  irparams[a].timer = 0;
			  irparams[a].rcvstate = STATE_MARK;
			} 
			else { // SPACE
			  if (irparams[a].timer > GAP_TICKS) {
				// big SPACE, indicates gap between codes
				// Mark current code as ready for processing
				// Switch to STOP
				// Don't reset timer; keep counting space width
				irparams[a].rcvstate = STATE_STOP;
			  } 
			}
			break;
		case STATE_STOP: // waiting, measuring gap
			if (irdata[a] == MARK) { // reset gap timer
			  irparams[a].timer = 0;
			}
			break;
	}
  }
}
void IRrecv::resume(int a) {
	irparams[a].rcvstate = STATE_IDLE;
	irparams[a].rawlen = 0;
}

// Decodes the received IR message
// Returns 0 if no data ready, 1 if data ready.
// Results of decoding are stored in results
void IRrecv::decode(decode_results *results) {
  for(int a = 0; a < NRECV; a++) {
	  if (irparams[a].rcvstate!= STATE_STOP) {
		results[a].decoder = 2;
		continue;
	  }
	  	  //Serial.println("bug11");
	  results[a].rawbuf = irparams[a].rawbuf;
	  results[a].rawlen = irparams[a].rawlen;


	#ifdef DEBUG
	  Serial.println("Attempting RC5 decode");
	#endif  
	  results[a].decoder = decodeRC5(results, a);
	  if (results[a].decoder == 0) {
			  //Serial.println("bug12");
		resume(a);
	  }
  }
}

// Gets one undecoded level at a time from the raw buffer.
// The RC5/6 decoding is easier if the data is broken into time intervals.
// E.g. if the buffer has MARK for 2 time intervals and SPACE for 1,
// successive calls to getRClevel will return MARK, MARK, SPACE.
// offset and used are updated to keep track of the current position.
// t1 is the time interval for a single bit in microseconds.
// Returns -1 for error (measured time interval is not a multiple of t1).
int IRrecv::getRClevel(decode_results *results, int *offset, int *used, int t1) {
  if (*offset >= results->rawlen) {
    // After end of recorded buffer, assume SPACE.
    return SPACE;
  }
  int width = results->rawbuf[*offset];
  int val = ((*offset) % 2) ? MARK : SPACE;
  int correction = (val == MARK) ? MARK_EXCESS : - MARK_EXCESS;

  int avail;
  if (MATCH(width, t1 + correction)) {
    avail = 1;
  } 
  else if (MATCH(width, 2*t1 + correction)) {
    avail = 2;
  } 
  else if (MATCH(width, 3*t1 + correction)) {
    avail = 3;
  } 
  else {
    return -1;
  }

  (*used)++;
  if (*used >= avail) {
    *used = 0;
    (*offset)++;
  }
#ifdef DEBUG
  if (val == MARK) {
    Serial.println("MARK");
  } 
  else {
    Serial.println("SPACE");
  }
#endif
  return val;   
}

long IRrecv::decodeRC5(decode_results *results, int a) {
  //Serial.println("bug1");
  if (irparams[a].rawlen < MIN_RC5_SAMPLES + 2) {
	//Serial.println("bug2");
    return ERR;
  }

  int offset = 1; // Skip gap space
  long data = 0;
  long type = 0;
  int used = 0;
  // Get start bits
  if (getRClevel(&results[a], &offset, &used, RC5_T1) != MARK) return ERR;
  if (getRClevel(&results[a], &offset, &used, RC5_T1) != SPACE) return ERR;
  if (getRClevel(&results[a], &offset, &used, RC5_T1) != MARK) return ERR;
  //Serial.println("bug3");
  // Get toggle bits
  if (getRClevel(&results[a], &offset, &used, RC5_T1) != MARK) return ERR;
  if (getRClevel(&results[a], &offset, &used, RC5_T1) != SPACE) return ERR;
  //Serial.println("bug4");
  // Get aircraft type bits
   int nbits;
  for (nbits = 0; nbits < 5; nbits++) {
    int levelA = getRClevel(&results[a], &offset, &used, RC5_T1); 
    int levelB = getRClevel(&results[a], &offset, &used, RC5_T1);
    if (levelA == SPACE && levelB == MARK) {
      // 1 bit
      type = (type << 1) | 1;
    } 
    else if (levelA == MARK && levelB == SPACE) {
      // zero bit
      type <<= 1;
    } 
    else {
	  //Serial.println("bug6");
      return ERR;
    } 
  } 
  //Serial.println("bug5");
  //Get aircraft sector
  for (nbits = 0; offset < irparams[a].rawlen; nbits++) {
    int levelA = getRClevel(&results[a], &offset, &used, RC5_T1); 
    int levelB = getRClevel(&results[a], &offset, &used, RC5_T1);
    if (levelA == SPACE && levelB == MARK) {
      // 1 bit
      data = (data << 1) | 1;
    } 
    else if (levelA == MARK && levelB == SPACE) {
      // zero bit
      data <<= 1;
    } 
    else {
	  //Serial.println("bug6");
      return ERR;
    } 
  }

  // Success
  results[a].value = data;
  results[a].type = type;
  return DECODED;
}