#ifndef _VSARDUINO_H_
#define _VSARDUINO_H_
//Board = Arduino Uno
#define __AVR_ATmega328p__
#define __AVR_ATmega328P__
#define _VMDEBUG 1
#define ARDUINO 105
#define ARDUINO_MAIN
#define __AVR__
#define __avr__
#define F_CPU 16000000L
#define __cplusplus
#define __inline__
#define __asm__(x)
#define __extension__
#define __ATTR_PURE__
#define __ATTR_CONST__
#define __inline__
#define __asm__ 
#define __volatile__

#define __builtin_va_list
#define __builtin_va_start
#define __builtin_va_end
#define __DOXYGEN__
#define __attribute__(x)
#define NOINLINE __attribute__((noinline))
#define prog_void
#define PGM_VOID_P int
            
typedef unsigned char byte;
extern "C" void __cxa_pure_virtual() {;}

//
//
void InitializeGame();
void GameOn();
void WriteEmptyBoard();
void GenerateFoodPosition(void);
Direction GetDirection(int keyChar, Direction currentValue);
bool MoveInDirection(Direction direction);
void CopyBoard(bool *source, bool *target);
COORD SetWormHeadPosition(Direction direction);
void SetWormTailPosition(COORD previousPosition);
bool FoodDetection(void);
bool CollisionDetection(void);
void PutFoodInActiveBuffer();
void WriteFrame(bool *board);
void GameOver(void);
void InitializeLcd();
short clearDisplay(void);
short getDisplayWidth(void);
short getDisplayHeight(void);
void drawFrame(void);
void RawWriteDisplay(bool *pixelBuffer);
void LcdWriteString(char *characters);
void LcdWriteCharacter(char character);
void LcdWriteData(byte data);
void LcdWriteCommand(byte command);
void LcdWrite(byte dc, byte value);
void LcdXY(int x, int y);

#include "C:\Program Files (x86)\Arduino\hardware\arduino\cores\arduino\arduino.h"
#include "C:\Program Files (x86)\Arduino\hardware\arduino\variants\standard\pins_arduino.h" 
#include "D:\Dropbox\Dev\Git\Masken\Masken.ino"
#include "D:\Dropbox\Dev\Git\Masken\PhilipsPCD8544.h"
#include "D:\Dropbox\Dev\Git\Masken\PhilipsPCD8544.ino"
#include "D:\Dropbox\Dev\Git\Masken\Types.h"
#include "D:\Dropbox\Dev\Git\Masken\font.h"
#endif
