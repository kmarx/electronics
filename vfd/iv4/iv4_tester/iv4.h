/*
 * iv4.h
 *
 *  Created on: Dec 25, 2020
 *      Author: kmarx
 */

#ifndef IV4_H_
#define IV4_H_

#define IV4_PIN_CATHODE1	1
#define IV4_PIN_CATHODE2	11
#define IV4_PIN_GRID		12
#define IV4_PIN_NC			22 // just fyi

/*
 * IV4_<bottom:middle:top|left:right|vert:horiz:diag>
 * IV4_<B|M|T><L|M|R><V|H|D>
 * B: bottom, T: top
 * L: left, R: right
 * V: vertical, H: horizontal, D: diagonal
 */
							// pin 1 is cathode/filiment
#define IV4_LDP (0)	// pin 2 - Left DP not supported yet
#define IV4_BLV (1 << 0)	// pin 3
#define IV4_BLH (1 << 1)	// pin 4
#define IV4_BLD (1 << 2)	// pin 5
#define IV4_BMV (1 << 3)	// pin 6
#define IV4_BRD (1 << 4)	// pin 7
#define IV4_BRH (1 << 5)	// pin 8
#define IV4_BRV (1 << 6)	// pin 9
#define IV4_RDP (0)			// pin 10 - Right DP not supported yet
							// pin 11 is cathode/filiment
							// pin 12 is grid
#define IV4_TRV (1 << 7)	// pin 13
#define IV4_TRH (1 << 8)	// pin 14
#define IV4_MRH (1 << 9)	// pin 15
#define IV4_TRD (1 << 10)	// pin 16
#define IV4_TMV (1 << 11)	// pin 17
#define IV4_TLD (1 << 12)	// pin 18
#define IV4_MLH (1 << 13)	// pin 19
#define IV4_TLH (1 << 14)	// pin 20
#define IV4_TLV (1 << 15)	// pin 21

#define IV4_BOX (IV4_TLH|IV4_TRH|IV4_TRV|IV4_BRV|IV4_BRH|IV4_BLH|IV4_BLV|IV4_TLV)
#define IV4_PLUS (IV4_TMV|IV4_MRH|IV4_BMV|IV4_MLH)
#define IV4_X (IV4_TLD|IV4_TRD|IV4_BRD|IV4_BLD)
#define IV4_ASTERISK ((IV4_PLUS)|(IV4_X))
#define IV4_ALL ((IV4_PLUS)|(IV4_X|IV4_BOX))

/*

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

#define IV4_CHAR_FIRST ' '		// First printable ascii char
#define IV4_CHAR_LAST '~'		// Last printable ascii char
#define IV4_PIN_NODEF 0 		// Used to terminate pin/segment lists
#define IV4_CHAR_NODEF (0xffff)	// Init to all pins on just to indicate something
#define IV4_NUM_PINS 22			// Cathode filaments: 2, Grid: 1, segments: 18, N/C: 1
#define IV4_MAX_SEGMENTS 16		// 18 really but omit left/right decimal points for now
#define IV4_NUM_CHARS	(IV4_CHAR_LAST - IV4_CHAR_FIRST)

class IV4Class {
	public:
		uint16_t segment(uint8_t pinNo);
		uint16_t getc(char c);
		uint16_t createChar(char c, ...);

	private:
		boolean initialized = false;
		uint16_t segments[IV4_NUM_PINS];		// Bit patterns for each segment (for now with holes for non-segment pins)
		uint16_t chars[IV4_NUM_CHARS];			// Flattened bit pattern for each individual character

		void init();
		void initializePins();
		void initializeChars();
};

extern IV4Class IV4;

#endif /* IV4_H_ */
