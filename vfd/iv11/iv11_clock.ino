#include <Arduino.h>
#include <math.h>
#include <SPI.h>
#include "iv11.h"


// SPI
#define	   CLOCK_PIN 13	// Goes to pin 11 of HC74595
#define	   LATCH_PIN 10 // Goes to pin 12 of HC74595
#define	   DATA_PIN 11	// Goes to pin 9 of HC74595


#define		HC74595_CLOCK_MHZ 31000000 // 31000000 // Per spec (I think?)
#define		MX_DUTY_CYLE	10			// How much to sleep between multiplexed tube display

int NumTubes = 2;

IV11 *Tubes;

typedef struct _MenuItem {
	const char *name;
	void (*callback)(void);
} MenuItem;

MenuItem Menu[] = {
		{"Test segments", testSegmentBits},
		{"Test hex bit pattern", testHexPattern},
		{"Test single numbers", testSingleDigitNumber},
		{"Test full number", testFullNumber},
		{"Test simple count", testSimpleCount}
};

void setup(){
  Serial.begin(9600); // debug
  logf("74HC595 clock=%u, $latch=%u, data=%u\n", CLOCK_PIN, LATCH_PIN, DATA_PIN);
  pinMode(CLOCK_PIN, OUTPUT);       // Output Pin Initializer
  pinMode(LATCH_PIN, OUTPUT);
  pinMode(DATA_PIN, OUTPUT);

  SPI.begin();

  Tubes = new IV11[NumTubes];

  //clearDisplay();
}

void loop()
{
	int option = getMenuOption();
	Menu[option].callback();
}

int getMenuOption() {
	int menuSize = sizeof(Menu)/sizeof(Menu[0]);
	while (true) {
		for (uint8_t i=0; i < menuSize; ++i) {
			lprintf("[%d] - %s\n", i+1, Menu[i].name);
		}
		int option = readNumber("Select menu option: ", 1, menuSize);
		return option-1;
	}
}

String readString(const char *prompt) {
	lprintf("%s", prompt);
	while (Serial.available() < 1) {
	   ; /* just wait */
	   /* Note: if you never get past this in Eclipse,
	    * Try deleting all ports from the "Serial monitor view" tab
	    * and try again
	    */
	}

	/* read the incoming byte
	*/
	String resp = Serial.readString();

	return resp;
}

long readNumber(const char *prompt, long min, long max) {
	int option;
	while (1) {
		String resp = readString(prompt);
//		option = resp.toInt();
		option = atol(resp.c_str());
		if (option < min || option > max) {
			lprintf("%s(%ld): Invalid. Number must be between %ld and %ld\n", resp.c_str(), option, min, max);
		} else {
			break;
		}
	}
	return option;
}

void testSegmentBits() {
	lprintf("Displaying all segments...\n");
	for (int i=0; i < 8; ++i) {
		uint8_t bits = (1 << i);
		logf("%d: bits=%#0x\n", i, bits);
		displayHiLo(0, bits);	// just use single tube
		delay(1000);
	}
	while (true) {
		int num = readNumber("Input a segment bit number, or -1 to return to menu: ", -1, 9);
		uint8_t bit = (1 << num);
		lprintf("Displaying bit=%d %#0x\n", num, bit);
		displayHiLo(0, bit); // Just use one tube anode for this
	}
}

void testHexPattern() {
	while (true) {
		String hexStr = readString("Input a hex bit pattern, or -1 to return to menu: ");
		int hex;;
		sscanf(hexStr.c_str(), "%x", &hex);
		uint8_t bits = hex;
		if (hex < 0) {
			return;
		}
		lprintf("Displaying '%s', decimal=%0d, bits=%#0x\n", hexStr.c_str(), bits, bits);
		displayHiLo(0, bits);
	}
}

void testSingleDigitNumber() {
	lprintf("Displaying all numbers...\n");
	for (int i=0; i < 10; ++i) {
		logf("Displaying number %d\n", i);
		showNumber(i, 100);
	}
	while (true) {
		int number = readNumber("Input a number to display, or -1 to return to menu: ", 0, 9);
		if (number < 0) {
			return;
		} else {
			lprintf("Displaying number=%d\n", number);
			showNumber(number, 100);
		}
	}
}

void testFullNumber() {
	int max = pow(10, NumTubes) - 1;
	lprintf("Display a number between 0 and %d\n", max);
	while (true) {
		long number = readNumber("Input a number to display, or -1 to return to menu: ", -1, max);
		long delay = readNumber("Input milliseconds delay for count, or -1 to return to menu: ", -1, 60L*1000L);
		if (number < 0 || delay < 0) {
			return;
		} else {
			lprintf("Displaying number=%d\n", number);
			showNumber(number, delay);
		}
	}
}

void testSimpleCount() {
	int cnt = pow(10, NumTubes);

	lprintf("Counting up to %d. Enter any key to abort...", cnt);
	for (int i=0; i <  cnt; ++i) {
		if (Serial.available()) {
			lprintf("Simple count interrupted. Returning...\n");
		}
		// count down
		showNumber(cnt-i, 100);
	}
	showNumber(0, 1000); // cheat and show 0 for a second
}

/*
 * Break given number into digits and display for durMs
 * milliseconds or forever if durMs < 0
 */
void showNumber(uint8_t number, long durMs) {
	uint8_t digits[NumTubes];

	//logf("showNumber(%d)\n", number);
	for (int tubeNum=0; tubeNum < NumTubes; ++tubeNum) {
		// Get successive digits. E.g.,
		// To get the '7' from 6712: (6712/100)%10 = (67)%10 = 7
		uint8_t digit = (number/(int)pow(10, tubeNum)) % 10;
		//logf("tubeNum=%d, digit=%d\n", tubeNum, digit);
		digits[tubeNum] = digit;
	}
	showDigits(digits, durMs);
}

/**
 * Show each respective digit on its corresponding tube for given
 * duration (in milliseconds) or forever if duration < 0
 */
void showDigits(uint8_t digits[], long durMs) {
	uint32_t start = millis();
	long end = start + durMs;
	logf("ms dur=%ld, start=%ld, end=%ld\n", durMs, start, end);
	while (durMs < 0 || (long)(end - millis()) > 0) {
		//long now = millis();
		//logf("ms start=%ld, end=%ld, now=%ld, dt=%ld\n", start, end, now, (end - now));
		if (Serial.available()) {
			logf("showDigits: Interrupted by user input. Returning...\n");
			Serial.readString(); // hack to flush input before return
			return;
		}
		for (int i=0; i < NumTubes; ++i) {
			//logf("digit[%d]=%d... ", i, digits[i]);
			display(i, digits[i]);
			delay(MX_DUTY_CYLE);
			//logf("\n");
		}
	}
}

/*
 * Display given digit on given tube number
 */
void display(int tubeNum, int digit) {

	IV11 tube = Tubes[tubeNum];

	tube.setNumber(digit);

	displayHiLo((1 << tubeNum), tube.getBits());
}

/*
 * Write raw bits to the two HC74595's.
 * One 595 is controls which tube's anode is enabled
 * The other controls the segments to light up for the given tube
 */
void displayHiLo(uint8_t anode, uint8_t segments) {

    // Get the shift register ready!
    digitalWrite(LATCH_PIN, LOW);

	SPI.beginTransaction(SPISettings(HC74595_CLOCK_MHZ, MSBFIRST, SPI_MODE0));
	//logf("anode=%#x, segments=%#x (%s)\n", anode, segments, byte2bin(segments));
	SPI.transfer(&segments,1);
	SPI.transfer(&anode,1);
	SPI.endTransaction();

	//delay(1000);	// debug since I don't have a data analyzer

	// Flush the data from the registers
    digitalWrite(LATCH_PIN, HIGH);
}

/*
 * ============== Local utils ======================
 */

/*
 * String representation of binary bits in a byte
 */
const char *byte2bin(int x)
{
    static char b[9];
    b[0] = '\0';

    int z;
    for (z = 128; z > 0; z >>= 1) {
        strcat(b, ((x & z) == z) ? "1" : "0");
    }

    return b;
}

/*
 * Hack: local printf
 */
void lprintf(const char fmt[], ...) {
	char msg[128];
	va_list argList;
	va_start(argList, fmt);
	vsprintf(msg, fmt, argList);
	va_end(argList);
	Serial.print(msg);
}

void logf(const char fmt[], ...) {
	char msg[128];
	va_list argList;
	va_start(argList, fmt);
	vsprintf(msg, fmt, argList);
	va_end(argList);
	Serial.print(msg);
}
