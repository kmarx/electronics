#include <Arduino.h>
#include <math.h>
#include <SPI.h>
#include "iv11.h"


// SPI
#define	   CLOCK_PIN 13
#define	   LATCH_PIN 10 // 12 - only works if 1) meter leed attached 2) delay(1) after set LOW
#define	   DATA_PIN 11


#define		HC74595_CLOCK_MHZ 31000000 // 31000000 // Per spec (I think?)
#define		MX_DUTY_CYLE	100			// How much to sleep between multiplexed tube display

int NumTubes = 1;

IV11 *Tubes;

typedef struct _MenuItem {
	const char *name;
	void (*callback)(void);
} MenuItem;

MenuItem Menu[] = {
		{"Test segments", testSegmentBits},
		{"Test numbers", testNumber},
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
		int option = readNumber("Select menu option: ");
		if (option >= menuSize) {
			lprintf("Invalid menu option!\n");
		}
		return option;
	}
}

int readNumber(const char *prompt) {
	lprintf("%s", prompt);
	while (Serial.available() < 1) {
	   /* just wait */ ;
	}

	/* read the incoming byte
	*/
	logf("Serial available! About to readString()\n");
	String resp = Serial.readString();
	int option = resp.toInt();
	lprintf("readNumber got string [%s]->option %d", resp.c_str(), option);
	return option;
}

void testSegmentBits() {
	lprintf("Displaying all segments...\n");
	for (int i=0; i < 8; ++i) {
		uint8_t bits = (1 << 0);
		logf("%d: bits=%#0x\n", i, bits);
		displayHiLo(0, bits);	// just use single tube
		delay(1000);
	}
	while (true) {
		int num = readNumber("Input a segment bit number, or -1 to return to menu: ");
		if (num < 0) {
			return;
		} else if (num > 9) {
			lprintf("%d: Invalid. Number must be between 0 and 9");
		} else {
			uint8_t bit = (1 << num);
			lprintf("Displaying bit=%d %#0x\n", num, bit);
			displayHiLo(0, bit); // Just use one tube anode for this
		}
	}
}

void testNumber() {
	lprintf("Displaying all numbers...\n");
	for (int i=0; i < 8; ++i) {
		logf("Displaying number %d\n", i);
		showNumber(i);
		delay(1000);
	}
	while (true) {
		int number = readNumber("Input a number to display, or -1 to return to menu: ");
		if (number < 0) {
			return;
		} else if (number > 9) {
			lprintf("%d: Invalid. Number must be between 0 and 9");
		} else {
			lprintf("Displaying number=%d\n", number);
			showNumber(number);
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
		showNumber(i);
		delay(1000);
	}
}

/*
 * Break given number into digits and display
 */
void showNumber(uint8_t number) {
	uint8_t digits[NumTubes];

	//logf("showNumber(%d)\n", number);
	for (int tubeNum=0; tubeNum < NumTubes; ++tubeNum) {
		// Get successive digits. E.g.,
		// To get the '7' from 6712: (6712/100)%10 = (67)%10 = 87
		uint8_t digit = (number/(int)pow(10, tubeNum)) % 10;
		//logf("tubeNum=%d, digit=%d\n", tubeNum, digit);
		digits[tubeNum] = digit;
	}
	showDigits(digits);
}

/*
 * Show each respective digit on its corresponding tube
 */
void showDigits(uint8_t digits[]) {
	for (int i=0; i < NumTubes; ++i) {
		logf("digit[%d]=%d... ", i, digits[i]);
		display(i, digits[i]);
		delay(MX_DUTY_CYLE);
		logf("\n");
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
void displayHiLo(int anode, int segments) {

    // Get the shift register ready!
    digitalWrite(LATCH_PIN, LOW);

	SPI.beginTransaction(SPISettings(HC74595_CLOCK_MHZ, MSBFIRST, SPI_MODE0));
	logf("anode=%#x, segments=%#x\n", anode, segments);
	//SPI.transfer(&anode,1);
	SPI.transfer(&segments,1);
	SPI.endTransaction();

	// Flush the data from the registers
    digitalWrite(LATCH_PIN, HIGH);
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
