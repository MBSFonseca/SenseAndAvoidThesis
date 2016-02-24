/*
 * IRremote
 * Version 0.11 August, 2009
 * Copyright 2009 Ken Shirriff
 * For details, see http://arcfn.com/2009/08/multi-protocol-infrared-remote-library.html
 *
 * Modified by Miguel Fonseca for sense and avoid prototype - January 2016
 */

#ifndef IRremote_h
#define IRremote_h

// The following are compile-time library options.
// If you change them, recompile the library.
// If DEBUG is defined, a lot of debugging output will be printed during decoding.
// TEST must be defined for the IRtest unittests to work.  It will make some
// methods virtual, which will be slightly slower, which is why it is optional.
//#define DEBUG
// #define TEST

#define NRECV 4
// Results returned from the decoder
class decode_results {
public:
  int decoder; //Decode state (Yes/No)
  int value; // received led
  int type; // type of aircraft
  int bits; // Number of bits in decoded value
  volatile unsigned int *rawbuf; // Raw intervals in .5 us ticks
  int rawlen; // Number of records in rawbuf.
};

// main class for receiving IR
class IRrecv
{
public:
  IRrecv(int *recvpin);
  void decode(decode_results *results);
  void enableIRIn();
  void resume(int a);
private:
  // These are called by decode
  int getRClevel(decode_results *results, int *offset, int *used, int t1);
  long decodeRC5(decode_results *results, int a);

} ;

// Only used for testing; can remove virtual for shorter code
#ifdef TEST
#define VIRTUAL virtual
#else
#define VIRTUAL
#endif

class IRsend
{
public:
  IRsend() {}
  //void sendRC5(unsigned long data, int nbits);
  void sendRC5(unsigned long data, int nbits, unsigned long temp);
  //void enableIROut(int khz);
  VIRTUAL void mark(int usec);
  VIRTUAL void space(int usec);
} ;

// Some useful constants

#define USECPERTICK 50  // microseconds per clock interrupt tick
#define RAWBUF 100 // Length of raw duration buffer

// Marks tend to be 100us too long, and spaces 100us too short
// when received due to sensor lag.
#define MARK_EXCESS 100

#endif
