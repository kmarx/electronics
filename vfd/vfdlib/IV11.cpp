/*
 * IV11.cpp
 *
 *  Created on: Jan 12, 2021
 *      Author: kmarx
 */

#include <Arduino.h>
#include "IV11.h"

constexpr uint8_t IV11::digits[]; // See iv11.h

IV11::IV11() {
	_setCurrent(0);
	_setErrorMsg("");
	_dumpBits();
	return;
}

IV11::~IV11() {
	return;
}
/* !static! */ boolean IV11::_isNumber(uint8_t bits) {
	bits &= ~(IV11_DP);	// Ignore decimal point
	for (uint8_t i=0; i < 9; ++i) {
		if (bits == IV11::digits[i]) {
			return true;
		}
	}
	return false;
}

/*
 * ----------------------------------------------------------------------
 * Private
 * ----------------------------------------------------------------------
 */

uint8_t IV11::_getCurrent() { return this->_current; }
uint8_t IV11::_setCurrent(uint8_t current) {
	uint8_t curr = _getCurrent();
	this->_current = current;
	return curr;
}
void IV11::_setErrorMsg(const char *errorMsg) {
	strncpy(this->_errorMsg, errorMsg, sizeof(this->_errorMsg));
}

/*
 * ----------------------------------------------------------------------
 * Public
 * ----------------------------------------------------------------------
 */

// Global actions
uint8_t IV11::clear() {
	return this->setBits(0);
}
uint8_t IV11::setAll() {
	return this->setBits(this->digits[8] | IV11_DP);
}
uint8_t IV11::get() {
	return this->_getCurrent();
}
boolean IV11::isError() {
	return strnlen(this->getErrorMsg(), IV11_MAX_ERRSZ) > 0;
}
char *IV11::getErrorMsg() {
	return this->_errorMsg;
}

// Character semantics
int IV11::setNumber(int n) {
	if (n < 0 || n > 9) {
		sprintf(this->_errorMsg, "%d: Not a valid number", n);
		logf("ERROR: %s", this->_errorMsg);
		return IV11_ERROR;
	}
	uint8_t bits = digits[n] | (isDP() ? IV11_DP : 0);
	return this->setBits(bits);
}

boolean IV11::isNumber() {
	return _isNumber(this->_getCurrent());
}

uint8_t IV11::setDP() {
	return this->setBits(this->_getCurrent() | IV11_DP);
}
uint8_t IV11::clearDP() {
	return this->setBits(this->_getCurrent() & ~IV11_DP);
}

boolean IV11::isDP() {
	return this->setBits(this->_getCurrent()) & IV11_DP;
}

// Bit level operations
uint8_t IV11::setBits(uint8_t bits) {
	logf("setBits(%#x)\n", bits);
	return this->_setCurrent(bits);
}
uint8_t IV11::getBits() {
	return this->_getCurrent();
}
uint8_t IV11::clearBits(uint8_t bits) {
	return this->setBits(_getCurrent() & ~(bits));
}

void IV11::logf(const char fmt[], ...) {
	char msg[128];
	va_list argList;
	va_start(argList, fmt);
	vsprintf(msg, fmt, argList);
	va_end(argList);
	Serial.print(msg);
}

void IV11::_dumpBits() {
	for (uint8_t i=0; i < sizeof(digits)/sizeof(digits[0]); ++i) {
		logf("Digit[%d]=%#x\n", i, digits[i]);
	}
}
