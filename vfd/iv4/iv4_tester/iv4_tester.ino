#include "iv4.h"

#define    CLOCK_PIN   2    // ATMEGA:  4   74HC595 SPI Clock Pin, SCK (pin 11 - SHCP shift register clock input)
#define    LATCH_PIN   3    // ATMEGA:  5   74HC595 SPI Latch Pin, RCK (pin 12 -  STCP storage register clock input)
#define    DATA_PIN    4    // ATMEGA:  6   74HC595 SPI Data Pin, SER (pin 14)

#define    NUM_SEGMENTS 16  // Number of segments in the VFD
#define    SEGMENT_DELAY 200
#define    REDRAW_DELAY  1000

int NumSegments = -1; // Overridden by prompt

void setup(){
  Serial.begin(9600); // debug
  pinMode(CLOCK_PIN, OUTPUT);       // Output Pin Initializer
  pinMode(LATCH_PIN, OUTPUT);
  pinMode(DATA_PIN, OUTPUT);

  //clearDisplay();
}

void loop(){

	//simpleSegmentTest();
	//testPatterns(1, REDRAW_DELAY);
	testChars();
//	test1pin();

}

void testChars() {
	clearDisplay();
	String userWord  = "";
	const char *testWord = "0123456789A";
	testWord = "a01";
	testWord = "a0123456789";
	testWord = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
//	testWord = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
//	testWord = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqr";
//	testWord = "abcdefghijklmnopqrstuvwxyz";
//	testWord = "stuvwxyz";
//	testWord = "0123456789ABCDEFGHIJKLMN";
	fuckf("FOREVER: testWord=%s, len=%d\n", testWord, strlen(testWord));
	char *testOutput = testWord;
	while (1) {
		if (Serial.available() > 0) {
			userWord = Serial.readString();
			fuckf("User entered '%s'\n", userWord);
			testOutput = userWord.c_str();
		} else {
//			testOutput = testWord;
		}
		fuckf("testOutput=%s, len=%d\n", testOutput, strlen(testOutput));
		for (unsigned int i=0; i<strlen(testOutput); ++i) {
			if (Serial.available() > 0) {
				break; // Get the user's input
			}
			fuckf("testOutput[%d]=%c (%d)\n", i, testOutput[i], testOutput[i]);
			displayBits(IV4.getc(testOutput[i]));
			delay(20);
//			clearDisplay();
		}
		delay(1000);
	}
}

void test1pin() {
	char dbg[128];

	uint16_t data = 0;
	//       1234567890123456
	//       6543210987654321
	//      65432109876543210 // shift left
	data = 0b0101010101010101;
//	data = 0b1010101010101010;
	//data = (IV4_BLV|IV4_BRV|IV4_TLV);

	//uint16_t data = 0x5555;
	uint8_t lo = data & 0xff;
	uint8_t hi = data >> 8;

	clearDisplay();
	sprintf(dbg, "writing %d: bit=0x%0x, lo=%0d,x%0x, hi=%0d,x%0x\n", data, data, lo, lo, hi, hi);
	Serial.print(dbg);
	displayBits(data);
	/*
	 *
	digitalWrite(LATCH_PIN, LOW);
	shiftOut(DATA_PIN, CLOCK_PIN, MSBFIRST, (hi << 0));
	shiftOut(DATA_PIN, CLOCK_PIN, MSBFIRST, (lo << 0));
	digitalWrite(LATCH_PIN, HIGH);
	 */

	delay(20*1000);

}

void simpleSegmentTest(){

  if (NumSegments < 0 && Serial.available() > 0) {
    Serial.print("How many segments (1-16): ");
    NumSegments = Serial.read();
    Serial.println("OK");
  } else {
    NumSegments = NUM_SEGMENTS;
  }
  Serial.print("NumSegments="); Serial.println(NumSegments);

  clearDisplay();
  Serial.println("------> Illuminate: START");
  for(int i = 1; i <= NumSegments; i++) {
    displaySegments(i);
  }
  delay(REDRAW_DELAY);

  Serial.println("------> UN-Illuminate: START");
  for(int i =  0; i <= NumSegments; i++) {
    displaySegments(NumSegments - i);
    //break;
  }

   delay(REDRAW_DELAY);
   Serial.println("------> FLASH");
   for (int i=0; i < 3; ++i) {
      displaySegments(NumSegments);
      delay(SEGMENT_DELAY);
      clearDisplay();
      delay(SEGMENT_DELAY);
   }

}

void testPatterns(int repetitions, int delayMs) {

	char dbg[128];
	/*
	*/
	int patterns[] = {IV4_BOX, IV4_PLUS, IV4_X, IV4_ASTERISK, IV4_ALL};

	for (int i=0; i < repetitions; ++i) {
		for (unsigned int j=0; j < sizeof(patterns)/sizeof(patterns[0]); ++j) {
			sprintf(dbg, "pattern[%0d]=x%0x\n", j, patterns[j]);
			Serial.print(dbg);
			displayBits(patterns[j]);
			delay(100);
			clearDisplay();
		}
		if (delayMs > 0) {
			delay(delayMs);
		}
		clearDisplay();
	}

    delay(1000);
	clearDisplay();
}

void clearDisplay() {
  Serial.println("========== Clear Display =========");
  displaySegments(0);
}

void flash(int numFlash) {
     Serial.println("------> FLASH");
   for (int i=0; i < numFlash; ++i) {
      displaySegments(NumSegments);
      delay(SEGMENT_DELAY);
      clearDisplay();
      delay(SEGMENT_DELAY);
   }
}

void displaySegments(int numSegments) {
    uint16_t data = getBits(numSegments);
    uint8_t hi = data >> 8;
    uint8_t lo = data & 0xff;

    // debug
    char dbg[128];
    sprintf(dbg, "numSegments=%02d, data=%02d (%0x) - high: %0x, low: %0x\n", numSegments, data, data, hi, lo);
    Serial.print(dbg);
    digitalWrite(LED_BUILTIN, HIGH);

    displayHiLo(hi, lo);

    delay(SEGMENT_DELAY);

    // debug
    digitalWrite(LED_BUILTIN, LOW);
}

void displayBits(int data) {
    uint8_t hi = data >> 8;
    uint8_t lo = data & 0xff;

    // debug
    char dbg[128];
    sprintf(dbg, "data=%02d (%0x) - high: %0x, low: %0x\n", data, data, hi, lo);
    Serial.print(dbg);
    digitalWrite(LED_BUILTIN, HIGH);

    displayHiLo(hi, lo);

    // debug
    digitalWrite(LED_BUILTIN, LOW);
}

void displayHiLo(int hi, int lo) {

    // Get the shift register ready!
    digitalWrite(LATCH_PIN, LOW);

    shiftOut(DATA_PIN, CLOCK_PIN, MSBFIRST, hi);
    shiftOut(DATA_PIN, CLOCK_PIN, MSBFIRST, lo);

    digitalWrite(LATCH_PIN, HIGH);

    delay(SEGMENT_DELAY);


}

uint16_t getBits(int numBits) {
  uint16_t bits = 0;
  for (int i=0; i < numBits; i++) {
    bits |= (1 << i);
  }
  return bits;
}

void fuckf(const char fmt[], ...) {
	char msg[128];
	va_list argList;
	va_start(argList, fmt);
	vsprintf(msg, fmt, argList);
	va_end(argList);
	Serial.print(msg);
}
