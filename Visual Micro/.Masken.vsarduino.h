#ifndef _VSARDUINO_H_
#define _VSARDUINO_H_
//Board = Arduino Uno
#define __AVR_ATmega328p__
#define __AVR_ATmega328P__
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
void autoPlay();
void InitializeGame();
void GameOn();
void WriteEmptyBoard();
void GenerateFoodPosition(void);
Direction GetDirection(int keyChar, Direction currentValue);
bool MoveInDirection(Direction direction);
COORD SetWormHeadPosition(Direction direction);
void SetWormTailPosition(COORD previousPosition);
bool FoodDetection(void);
bool CollisionDetection(void);
void PutFoodInActiveBuffer();
void GameOver(void);
void drawBuffer(short x, short y);
void clearBuffer(short x, short y);
void bufferXY(short x, short y);
void bufferWriteData(byte data);
void sendBufferToLcd();
void clearBuffer();

#include "C:\Program Files (x86)\Arduino\hardware\arduino\cores\arduino\arduino.h"
#include "C:\Program Files (x86)\Arduino\hardware\arduino\variants\standard\pins_arduino.h" 
#include "D:\Dropbox\Dev\Git\Masken\Masken.ino"
#include "D:\Dropbox\Dev\Git\Masken\Font.h"
#include "D:\Dropbox\Dev\Git\Masken\PhilipsPCD8544.cpp"
#include "D:\Dropbox\Dev\Git\Masken\PhilipsPCD8544.h"
#include "D:\Dropbox\Dev\Git\Masken\Timer.cpp"
#include "D:\Dropbox\Dev\Git\Masken\Timer.h"
#include "D:\Dropbox\Dev\Git\Masken\Types.h"
#endif
