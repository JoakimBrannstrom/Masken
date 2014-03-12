
#pragma once

#include <conio.h>
//#include <SDKDDKVer.h>
#include <stdio.h>
#include <stdlib.h>
#include <tchar.h>
#include <windows.h>

#include <cstdlib>
#include <fstream>
#include <iostream>
#include <string>

using namespace std;

enum Direction { None, Up, Right, Down, Left };

void GameOn();
bool GameOver(void);

void WriteFrame(PCHAR_INFO board);
void SwitchBuffers(void);
void PutWormInActiveBuffer();
void PutFoodInActiveBuffer();
void WriteTextMessageToBuffer(std::wstring message, int yOffset);
void SetWormPosition(Direction direction);
bool CollisionDetection(void);
bool FoodDetection(void);
void GenerateFoodPosition(void);
void CreateBoard(PCHAR_INFO board);
void CopyBoard(PCHAR_INFO source, PCHAR_INFO target);
bool MoveInDirection(Direction direction);
Direction GetDirection(int keyChar, Direction currentValue);
