/*
 * morseaircraft - Library to generate Morse code that identifies an aircraft's category.
 * Created by Miguel Saraiva Fonseca, December, 2015.
 * Released into the public domain.
 */

#include "Arduino.h"
#include "morseaircraft.h"

MorseAircraft::MorseAircraft(char type[])
{
	interval = 171; //define the period of each element of the morse code 171ms
	previousMillis = 0;
	if(type[0]=='U') //if aircraft is unmanned
	{
		switch(type[1]) //check type of aircraft to select correct Morse message which is converted to decimal to store in a 'long'
		{
			case 'A':
				message = 29813; //(0000000111010001110101 = UA)
				order = 21;
				number = 14;
				break;
			case 'B':
				message = 351349; //(00000001010101110001110101 = UB)
				order = 25;
				number = 15;
				break;
			case 'C':
				message = 1530997; //(0000000101110101110001110101 = UC)
				order = 27;
				number = 16;
				break;
			case 'D':
				message = 89205; //(000000010101110001110101 = UD)
				order = 23;
				number = 17;
				break;
			case 'E':
				message = 1141; //(000000010001110101 = UE)
				order = 17;
				number = 18;
				break;
			case 'H':
				message = 87157; //(000000010101010001110101 = UH)
				order = 23;
				number = 19;
				interval = 175; //special case, needs different interval
				break;
			case 'K':
				message = 482421; //(00000001110101110001110101 = UK)
				order = 25;
				number = 20;
				break;
			case 'L':
				message = 357493; //(00000001010111010001110101 = UL)
				order = 25;
				number = 21;
				break;
			case 'M':
				message = 121973; //(000000011101110001110101 = UM)
				order = 23;
				number = 22;
				break;
			case 'N':
				message = 23669; //(0000000101110001110101 = UN)
				order = 21;
				number = 23;
				break;
			case 'Q':
				message = 7724149; //(000000011101011101110001110101 = UQ)
				order = 29;
				number = 24;
				break;
			case 'R':
				message = 95349; //(000000010111010001110101 = UR)
				order = 23;
				number = 25;
				break;
			case 'S':
				message = 21621; //(0000000101010001110101 = US)
				order = 21;
				number = 26;
				break;
			case 'T':
				message = 7285; //(00000001110001110101 = UT)
				order = 19;
				number = 27;
				break;
		}
	}
	else //manned aircraft
	{
		switch(type[0])
		{
			case 'A':
				message = 29; //(000000011101 = A)
				order = 11;
				number = 0;
				break;
			case 'B':
				message = 343; //(0000000101010111 = B)
				order = 15;
				number = 1;
				break;
			case 'C':
				message = 1495; //(000000010111010111 = C)
				order = 17;
				number = 2;
				break;
			case 'D':
				message = 87; //(00000001010111 = D)
				order = 13;
				number = 3;
				break;
			case 'E':
				message = 1; //(00000001 = E)
				order = 7;
				number = 4;
				break;
			case 'H':
				message = 85; //(00000001010101 = H)
				order = 13;
				number = 5;
				break;
			case 'K':
				message = 471; //(0000000111010111 = K)
				order = 15;
				number = 6;
				break;
			case 'L':
				message = 349; //(0000000101011101 = L)
				order = 15;
				number = 7;
				break;
			case 'M':
				message = 119; //(00000001110111 = M)
				order = 13;
				number = 8;
				break;
			case 'N':
				message = 23; //(000000010111 = N)
				order = 11;
				number = 9;
				break;
			case 'Q':
				message = 7543; //(00000001110101110111 = Q)
				order = 19;
				number = 10;
				break;
			case 'R':
				message = 93; //(00000001011101 = R)
				order = 13;
				number = 11;
				break;
			case 'S':
				message = 21; //(000000010101 = S)
				order = 11;
				number = 12;
				break;
			case 'T':
				message = 7; //(0000000111 = T)
				order = 9;
				number = 13;
				interval = 150; //special case, needs different interval
				break;
		}
	}
}

void MorseAircraft::flash()
{
	unsigned long currentMillis = millis(); //save current time
	if (currentMillis - previousMillis >= interval) //check if interval as passed
	{
		digitalWrite(COLLISIONLED,bitRead(message,0)); //turn led on or off (depends on the bit)
		previousMillis = currentMillis;
		temp = message >> 1; //rotate bit 00001 -> 00000
		if (bitRead(message,0)) bitWrite(temp,order,1); //rotate bit: 00001 -> 10000
		message = temp;
//   	Serial.print("character: ");
//  	Serial.println(character, BIN);
	}
}