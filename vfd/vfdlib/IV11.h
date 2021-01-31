/*
 * IV11.h
 *
 *  Created on: Jan 12, 2021
 *      Author: kmarx
 */

#include <Arduino.h>
#ifndef IV11_H_
#define IV11_H_

#define IV11_ERROR			0xffff
#define IV11_MAX_ERRSZ		128

#define IV11_PIN_CATHODE1	1
#define IV11_PIN_CATHODE2	11
#define IV11_PIN_GRID		2

							// pin 1 is cathode/filament
							// pin 2 is grid
#define IV11_BR (1 << 0)	// pin 3
#define IV11_DP (1 << 1)	// pin 4
#define IV11_TR (1 << 2)	// pin 5
#define IV11_T	(1 << 3)	// pin 6
#define IV11_M	(1 << 4)	// pin 7
#define IV11_TL (1 << 5)	// pin 8
#define IV11_BL (1 << 6)	// pin 9
#define IV11_B	(1 << 7)	// pin 10
							// pin 11 is cathode/filament
/*
 * SEGMENT PIN OUT DIAGRAM

             A
             6
      ---------------
     |               |
     |               |
  F 8|               |5 B
     |       G       |
     |       7       |
      ---------------
     |               |
     |               |
  E 9|               |3 C
     |               |
     |               |
      ---------------
            10       (DP)
            D         4 H

 */

class IV11 {
private:
	uint8_t _current;
	char _errorMsg[IV11_MAX_ERRSZ];

	uint8_t _getCurrent();
	uint8_t _setCurrent(uint8_t current);
	void _setErrorMsg(const char *errorMsg);
	void logf(const char fmt[], ...);
	void  _dumpBits();

	static constexpr uint8_t digits[] = {
			(IV11_T|IV11_TR|IV11_BR|IV11_B|IV11_BL|IV11_TL|0     ),	// 0
			(0     |IV11_TR|IV11_BR|0     |0      |0      |0     ),	// 1
			(IV11_T|IV11_TR|0      |IV11_B|IV11_BL|0      |IV11_M),	// 2
			(IV11_T|IV11_TR|IV11_BR|IV11_B|0      |0      |IV11_M),	// 3
			(0     |IV11_TR|IV11_BR|0     |0      |IV11_TL|IV11_M),	// 4
			(IV11_T|0      |IV11_BR|IV11_B|0      |IV11_TL|IV11_M),	// 5
			(IV11_T|0      |IV11_BR|IV11_B|IV11_BL|IV11_TL|IV11_M),	// 6
			(IV11_T|IV11_TR|IV11_BR|0     |0      |0      |0     ),	// 7
			(IV11_T|IV11_TR|IV11_BR|IV11_B|IV11_BL|IV11_TL|IV11_M),	// 8
			(IV11_T|IV11_TR|IV11_BR|IV11_B|0      |IV11_TL|IV11_M) 	// 9
	};
	static boolean _isNumber(uint8_t bits);

public:
	IV11();
	virtual ~IV11();

	// Global actions
	uint8_t clear();
	uint8_t setAll();
	uint8_t get();
	boolean isError();
	char *getErrorMsg();

	// Character semantics
	int setNumber(int n);
	boolean isNumber();

	uint8_t setDP();
	uint8_t clearDP();
	boolean isDP();

	// Bit level operations
	uint8_t setBits(uint8_t bits);
	uint8_t getBits();
	uint8_t clearBits(uint8_t bits);
};

#endif /* IV11_H_ */
