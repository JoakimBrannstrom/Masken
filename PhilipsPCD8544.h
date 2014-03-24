// PhilipsPCD8544.h

#ifndef _PHILIPSPCD8544_h
#define _PHILIPSPCD8544_h

#ifdef __cplusplus
// Arduino C: undefined reference to ...
// http://stackoverflow.com/questions/19635007/arduino-c-undefined-reference-to-readarms
//extern "C" {
#endif

#include "font.h";
#include <Arduino.h>

#define RST 12
#define CE 11
#define DC 10
#define DIN 9
#define CLK 8

#define DisplayWidth 84
#define DisplayHeight 48
#define DisplaySizeInBytes 504	// 84 * 48 / 8

void initializeLcd();
void clearDisplay(void);
void drawFrame(void);
void drawBlock(short x, short y, short width, short height);
void LcdWriteString(char *characters);
void LcdWriteCharacter(char character);
void LcdWriteData(byte data);
void LcdWriteCommand(byte command);
void LcdWrite(byte dc, byte value);
void LcdXY(int x, int y);

#ifdef __cplusplus
//}
#endif
#endif

