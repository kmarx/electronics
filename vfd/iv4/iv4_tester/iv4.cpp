/*
 * iv4.cpp
 *
 *  Created on: Dec 27, 2020
 *      Author: kmarx
 */
#include <stdio.h>
#include <Arduino.h>
#include "iv4.h"

/* ========================================
 * === Local helper method declarations ===
 * ========================================
 */
static uint16_t calculatePinBits(uint8_t pinNum);
static uint8_t getCharIndex(char c);
void logf(const char fmt[], ...);

void info(const char errMsg[]);
static void error(const char errMsg[]);
static void warn(const char errMsg[]);

/* ========================================
 * === Class methods
 * ========================================
 */
void IV4Class::init() {
	info("IV4 Initializing...");
	initializePins();
	initializeChars();
}

uint16_t IV4Class::segment(uint8_t pinNum) {
	if (!initialized) {
		init();
	}
	return segments[pinNum];
}

uint16_t IV4Class::getc(char c) {
	if (!initialized) {
		init();
		initialized = true;
	}
	uint8_t idx = getCharIndex(c);
	logf("getc=%c index=%d, bits=%#x\n", c, idx, chars[idx]);
	return chars[getCharIndex(c)];
}

/* ========================================
 * === Local helper methods
 * ========================================
 */

/**
 * Initialize static pins array with bit corresponding pattern
 * That is, the bit pattern to write on the shift register to light up
 * the segment on the given pin
 */
void IV4Class::initializePins() {
	info("IV4 InitializePins");
	segments[IV4_PIN_CATHODE1] = IV4_PIN_NODEF;
	segments[IV4_PIN_CATHODE2] = IV4_PIN_NODEF;
	segments[IV4_PIN_GRID] = IV4_PIN_NODEF;

	// First 7 pins starting at first cathode pin 3 (not including left DP=2, right DP=10)
	for (uint16_t pin = 3; pin < 10; ++pin) {
		segments[pin] = calculatePinBits(pin);
	}
	// Cathode pins starting at 13
	for (uint16_t pin = 13; pin < IV4_NUM_PINS; ++pin) {
		segments[pin] = calculatePinBits(pin);
	}
}

/*
 * Create default character set
 *
 * Handy pinout diagram to help with character segment list definitions

       20       14
      ------- -------
     | \     |     / |
     |  \18  |  16/  |
   21|   \   |   /   |13
     |    \ 17  /    |
     | 19  \ | /  15 |
      ------- -------
     |     / | \     |
     |   5/  |  \7   |
    3|   /   |   \   |9
     |  /   6|    \  |
     | /     |     \ |
      ------- -------
   (DP)    4      8  (DP)
    2                 10

 */
void IV4Class::initializeChars() {
	info("IV4 InitializeChars");
	for (int i=0; i < IV4_NUM_CHARS; ++i) {
		chars[i] = IV4_PIN_NODEF;
	}
	// Pin number lists have no particular ordering
	createChar(' ', 4, 8, IV4_PIN_NODEF); // "underbar" for now just, well, for now
	createChar('?', 20, 14, 13, 15, 6, IV4_PIN_NODEF);

	createChar('0', 20, 14, 13, 9, 8, 4, 3, 21, IV4_PIN_NODEF);
	createChar('1', 16, 13, 9, IV4_PIN_NODEF);
	createChar('2', 20, 14, 13, 15, 19, 3, 4, 8, IV4_PIN_NODEF);
	createChar('3', 20, 14, 13, 15, 9, 4, 8, IV4_PIN_NODEF);
	createChar('4', 21, 19, 15, 13, 9, IV4_PIN_NODEF);
	createChar('5', 14, 20, 21, 19, 7, 4, 8, IV4_PIN_NODEF);
	createChar('6', 14, 20, 21, 3, 4, 8, 9, 15, 19, IV4_PIN_NODEF);
	createChar('7', 20, 14, 16, 5, IV4_PIN_NODEF);
	createChar('8', 20, 14, 13, 9, 8, 4, 3, 21, 19, 15, IV4_PIN_NODEF);
	createChar('9', 20, 14, 13, 9, 8, 4, 21, 19, 15, IV4_PIN_NODEF);

	createChar('a', 19, 5, 4, 6, 8, IV4_PIN_NODEF);
	createChar('b', 21, 3, 4, 8, 9, 15, 19, IV4_PIN_NODEF);
	createChar('c', 19, 3, 4, IV4_PIN_NODEF);
	createChar('d', 13, 9, 8, 6, 15, IV4_PIN_NODEF);
	createChar('e', 5, 19, 3, 4, IV4_PIN_NODEF);
	createChar('f', 14, 17, 6, 19, 15, IV4_PIN_NODEF);
	createChar('g', 17, 14, 13, 9, 8, 15, IV4_PIN_NODEF);
//	createChar('g', 14, 16, 17, 15, 9, 8, 6, IV4_PIN_NODEF); // cytstal type font
	createChar('h', 21, 3, 19, 6, IV4_PIN_NODEF);
	createChar('i', 6, IV4_PIN_NODEF);
	createChar('j', 17, 6, 4, 3, IV4_PIN_NODEF);
	createChar('k', 17, 6, 16, 7, IV4_PIN_NODEF);
	createChar('l', 17, 6, IV4_PIN_NODEF);
	createChar('m', 3, 19, 6, 15, 9, IV4_PIN_NODEF);
	createChar('n', 3, 19, 6, IV4_PIN_NODEF);
	createChar('o', 3, 19, 6, 4, IV4_PIN_NODEF);
	createChar('p', 17, 6, 14, 13, 15, IV4_PIN_NODEF);
	createChar('q', 17, 14, 13, 9, 15, IV4_PIN_NODEF);
	createChar('r', 3, 19, IV4_PIN_NODEF);
	createChar('s', 20, 21, 19, 6, 4, IV4_PIN_NODEF);
	createChar('t', 21, 3, 4, 19, IV4_PIN_NODEF);
	createChar('u', 3, 4, 8, 9, IV4_PIN_NODEF);
	createChar('v', 7, 9, IV4_PIN_NODEF);
	createChar('w', 3, 5, 7, 9, IV4_PIN_NODEF);
	createChar('x', 19, 15, 6, 4, 8, IV4_PIN_NODEF);
	createChar('y', 17, 15, 13, 9, 8, IV4_PIN_NODEF);
	createChar('z', 19, 5, 4, IV4_PIN_NODEF);

	createChar('A', 3, 21, 20, 14, 13, 9, 19, 15, IV4_PIN_NODEF);
	createChar('B', 20, 14, 13, 9, 8, 4, 17, 6, 15, IV4_PIN_NODEF);
	createChar('C', 14, 20, 21, 3, 4, 8, IV4_PIN_NODEF);
	createChar('D', 20, 14, 13, 9, 8, 4, 17, 6, IV4_PIN_NODEF);
	createChar('E', 3, 21, 20, 14, 19, 4, 8, IV4_PIN_NODEF);
	createChar('F', 3, 21, 20, 14, 19, IV4_PIN_NODEF);
	createChar('G', 14, 20, 21, 3, 4, 8, 9, 15, IV4_PIN_NODEF);
	createChar('H', 21, 3, 13, 9, 19, 15, IV4_PIN_NODEF);
	createChar('I', 20, 14, 17, 6, 4, 8, IV4_PIN_NODEF);
	createChar('J', 3, 4, 8, 9, 13, IV4_PIN_NODEF);
	createChar('K', 21, 3, 16, 19, 7, IV4_PIN_NODEF);
	createChar('L', 21, 3, 4, 8, IV4_PIN_NODEF);
	createChar('M', 3, 21, 18, 16, 13, 9, IV4_PIN_NODEF);
	createChar('N', 3, 21, 18, 7, 13, 9, IV4_PIN_NODEF);
	createChar('O', 20, 14, 13, 9, 8, 4, 3, 21, IV4_PIN_NODEF);
	createChar('P', 3, 21, 20, 14, 13, 15, 19, IV4_PIN_NODEF);
	createChar('Q', 20, 14, 13, 9, 8, 4, 3, 21, 7, IV4_PIN_NODEF);
	createChar('R', 3, 21, 20, 14, 13, 15, 19, 7, IV4_PIN_NODEF);
	createChar('S', 14, 20, 21, 19, 15, 9, 8, 4, IV4_PIN_NODEF);
	createChar('T', 20, 14, 17, 6, IV4_PIN_NODEF);
	createChar('U', 21, 3, 4, 8, 9, 13, IV4_PIN_NODEF);
	createChar('V', 21, 3, 5, 16, IV4_PIN_NODEF);
	createChar('W', 21, 3, 5, 7, 9, 13, IV4_PIN_NODEF);
	createChar('X', 18, 7, 16, 5, IV4_PIN_NODEF);
	createChar('Y', 18, 16, 6, IV4_PIN_NODEF);
	createChar('Z', 20, 14, 16, 5, 4, 8, IV4_PIN_NODEF);
}

static uint8_t getCharIndex(char c) {
	return c - IV4_CHAR_FIRST;
}

/*
 * Returns the bit pattern for pinNum
 */
uint16_t calculatePinBits(uint8_t pinNum) {
	uint16_t bits = 0;

	boolean validPin = true;
	switch (pinNum) {
	case IV4_PIN_CATHODE1:
	case IV4_PIN_CATHODE2:
	case IV4_PIN_GRID:
		validPin = false;
		break;
	default:
		if (pinNum > 21) {
			validPin = false;
		}
		break;
	}
	if (!validPin) {
		logf("ERROR: %02d: Invalid PIN number", pinNum);
	}

	// See iv4.h
	if (pinNum < 10) {
		// First valid pin is #3. Gets bit[0]. Offset = 3
		bits = (1 << (pinNum - 3));
	} else {
		// Next valid pin is #13. Gets bit[7]. Offset = 13-7 = 6
		bits = (1 << (pinNum - 6));
	}

	//logf("pin[%d] -> bits:%#x\n", pinNum, bits);
	return bits;
}

uint16_t IV4Class::createChar(char c, ...) {
	int numSegments = 0;
	uint8_t pins[IV4_MAX_SEGMENTS];

	// Count the segments. Start at 1 since pin1 is a given
	va_list argList;
	va_start(argList, c);
	for(int i=0; i < IV4_MAX_SEGMENTS; ++i) {
		int pinNo = va_arg(argList, int);
		if (pinNo == IV4_PIN_NODEF) {
			break;
		}
	    pins[i] = pinNo;
		//logf("%c: pin[%d]=%d\n", c, i, pins[i]);
	    ++numSegments;
	}
	va_end(argList);

	if (numSegments >= IV4_MAX_SEGMENTS) {
		logf("%d: Too many pins?", numSegments);
	}
	uint16_t bits = 0;
	uint16_t charIdx = getCharIndex(c);
	for (int i=0; i < numSegments; ++i) {
		uint16_t pinBits = calculatePinBits(pins[i]);
		bits |= pinBits;
		//logf("%c: idx=%d, segment[%d]=%#x, pinBits=%#x\n", c, charIdx, i, bits, pinBits);
	}
	if (chars[charIdx] != IV4_PIN_NODEF) {
		logf("WARN: Re-defining character %c: segments=%d, bits=%#x (%u). Was %#x\n", c, numSegments, bits, bits);
	}
	logf("createChar('%c') idx=%d, segments=%d, bits=%#x (%u)\n", c, charIdx, numSegments, bits, bits);
	chars[charIdx] =  bits;
	return bits;
}

/**
 * TODO - these log calls should all take va_args. It's a mess
 */
void info(const char errMsg[]) {
	logf("INFO: %s\n", errMsg);
}
void warn(const char errMsg[]) {
	logf("WARN: %s\n", errMsg);
}
void error(const char errMsg[]) {
	logf("ERROR: %s\n", errMsg);
	// TODO  do something significant here
}

void logf(const char fmt[], ...) {
//	return;
	char msg[128];
	va_list argList;
	va_start(argList, fmt);
	vsprintf(msg, fmt, argList);
	va_end(argList);
	Serial.print(msg);
}
/*
*/

IV4Class IV4;
