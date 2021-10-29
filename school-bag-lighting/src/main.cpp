// A basic everyday NeoPixel strip test program.

// NEOPIXEL BEST PRACTICES for most reliable operation:
// - Add 1000 uF CAPACITOR between NeoPixel strip's + and - connections.
// - MINIMIZE WIRING LENGTH between microcontroller board and first pixel.
// - NeoPixel strip's DATA-IN should pass through a 300-500 OHM RESISTOR.
// - AVOID connecting NeoPixels on a LIVE CIRCUIT. If you must, ALWAYS
//   connect GROUND (-) first, then +, then data.
// - When using a 3.3V microcontroller with a 5V-powered NeoPixel strip,
//   a LOGIC-LEVEL CONVERTER on the data line is STRONGLY RECOMMENDED.
// (Skipping these may work OK on your workbench but can fail in the field)

#include "SPI.h"
#include <Adafruit_GFX.h>
#include <Adafruit_I2CDevice.h>
#include <Adafruit_NeoMatrix.h>
#include <Adafruit_NeoPixel.h>
#include "Fonts/Picopixel.h"

//https://github.com/adafruit/Adafruit_Learning_System_Guides/blob/main/Trinket_Gemma_Space_Invader_Pendant/Trinket_Gemma_Space_Invader_Pendant.ino
const uint8_t PROGMEM alien1[][8] = {
	{// Frames 3 & 4 for alien #1 are duplicates of frames 1 & 2.
	 // Rather than list them 'the tall way' again, the lines are merged here...
	 B00011000, B00111100, B01111110, B11011011, B11111111, B00100100, B01011010, B10100101},
	{B00011000, B00111100, B01111110, B11011011, B11111111, B00100100, B01011010, B01000010}};
const uint8_t PROGMEM alien2[][8] = {
	{// Frames 3 & 4 for alien #2 are duplicates of frames 1 & 2
	 B00000000, B00111100, B01111110, B11011011, B11011011, B01111110, B00100100, B11000011},
	{B00111100, B01111110, B11011011, B11011011, B01111110, B00100100, B00100100, B00100100}};
const uint8_t PROGMEM alien3[][8] = {
	{// Frames are duplicated as with prior aliens
	 B00100100, B00100100, B01111110, B11011011, B11111111, B11111111, B10100101, B00100100},
	{B00100100, B10100101, B11111111, B11011011, B11111111, B01111110, B00100100, B01000010}};
const uint8_t PROGMEM alien4[][8] = {
	{// Frames 5-8 are duplicates of 1-4, lines merged for brevity
	 B00111100, B01111110, B00110011, B01111110, B00111100, B00000000, B00001000, B00000000},
	{B00111100, B01111110, B10011001, B01111110, B00111100, B00000000, B00001000, B00001000},
	{B00111100, B01111110, B11001100, B01111110, B00111100, B00000000, B00000000, B00001000},
	{B00111100, B01111110, B01100110, B01111110, B00111100, B00000000, B00000000, B00000000}};

// Which pin on the Arduino is connected to the NeoPixels?
// On a Trinket or Gemma we suggest changing this to 1:
#define LED_PIN PA8

Adafruit_NeoMatrix matrix = Adafruit_NeoMatrix(32, 8, LED_PIN, NEO_MATRIX_TOP + NEO_MATRIX_LEFT + NEO_MATRIX_COLUMNS + NEO_MATRIX_ZIGZAG, NEO_GRB + NEO_KHZ800);

// setup() function -- runs once at startup --------------------------------
void setup()
{
	matrix.begin();			  // INITIALIZE NeoPixel strip object (REQUIRED)
	matrix.show();			  // Turn OFF all pixels ASAP
	matrix.setBrightness(50); // Set BRIGHTNESS to about 1/5 (max = 255)
	randomSeed(analogRead(0));
}

// Rainbow-enhanced theater marquee. Pass delay time (in ms) between frames.
void theaterChaseRainbow(int wait)
{
	int firstPixelHue = 0; // First pixel starts at red (hue 0)
	for (int a = 0; a < 10; a++)
	{ // Repeat 10 times...
		for (int b = 0; b < 3; b++)
		{					//  'b' counts from 0 to 2...
			matrix.clear(); //   Set all pixels in RAM to 0 (off)
			// 'c' counts up from 'b' to end of strip in increments of 3...
			for (int c = b; c < matrix.numPixels(); c += 3)
			{
				// hue of pixel 'c' is offset by an amount to make one full
				// revolution of the color wheel (range 65536) along the length
				// of the strip (strip.numPixels() steps):
				int hue = firstPixelHue + c * 65536L / matrix.numPixels();
				uint32_t color = matrix.gamma32(matrix.ColorHSV(hue)); // hue -> RGB
				matrix.setPixelColor(c, color);						   // Set pixel 'c' to value 'color'
			}
			matrix.show();				 // Update strip with new contents
			delay(wait);				 // Pause for a moment
			firstPixelHue += 65536 / 90; // One cycle of color wheel over 90 frames
		}
	}
}

void showText(int wait)
{
	matrix.clear();
	matrix.setFont(&Picopixel);
	for (int j = 0; j < 4; j++)
	{
		for (int i = 255; i > 0; i -= 5)
		{
			matrix.setCursor(6, 5);
			matrix.setTextColor(j & 1 ? matrix.Color(i, i, i) : matrix.Color(i, i, 0));
			matrix.print("LET OP");
			matrix.show();
			delay(10);
		}
	}
}

void showPacman()
{
	static int curpos = 0;
	static byte state = 0;
	int shift = random(32);
	int direction = random(1000) < 500 ? -1 : 1;
	int alientype = random(1000) % 4;
	const byte(*bitmap)[8];
	int spriteCount = 0;
	word color = 0;
	switch (alientype)
	{
	case 0:
		bitmap = alien1;
		spriteCount = 2;
		color = matrix.Color(127, 0, 0);
		break;
	case 1:
		bitmap = alien2;
		spriteCount = 2;
		color = matrix.Color(127, 127, 0);
		break;
	case 2:
		bitmap = alien3;
		spriteCount = 2;
		color = matrix.Color(127, 127, 127);
		break;
	case 3:
		bitmap = alien4;
		spriteCount = 4;
		color = matrix.Color(0, 127, 0);
		break;
	default:
		break;
	}
	for (int i = 0; i < shift; i++)
	{
		state = (state == 2 ? 0 : state + 1);
		matrix.clear();
		curpos += direction;
		curpos = curpos % 32;
		matrix.drawBitmap(curpos, 0, bitmap[state], 8, 8, color);
		matrix.show();
		delay(75);
	}
}

word hsvColor(word hue)
{
	uint32_t wrgb = matrix.gamma32(matrix.ColorHSV(hue));
	uint32_t r = (wrgb >> 19) & 0x1F;
	uint32_t g = (wrgb >> 10) & 0x3F;
	uint32_t b = (wrgb >> 3) & 0x1F;
	return (r << 11) + (g << 5) + b;
}

void rainbowPacman(int wait)
{
	for (long firstPixelHue = 0; firstPixelHue < 65536; firstPixelHue += 256)
	{
		matrix.clear();
		int subsprite = (firstPixelHue >> 11) & 1;
		matrix.drawBitmap(2, 0, alien1[subsprite], 8, 8, hsvColor(firstPixelHue));
		matrix.drawBitmap(12, 0, alien2[subsprite], 8, 8, hsvColor(firstPixelHue + 20000));
		matrix.drawBitmap(22, 0, alien3[subsprite], 8, 8, hsvColor(firstPixelHue + 40000));
		matrix.show(); // Update strip with new contents
		delay(wait);   // Pause for a moment
	}
}

void loop()
{
	theaterChaseRainbow(50); // Rainbow-enhanced theaterChase variant
	showText(50);
	for (int i = 0; i < 10; i++)
	{
		showPacman();
	}
	rainbowPacman(50);
}
