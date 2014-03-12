#include "stdafx.h"
#include <conio.h>
#include <stdio.h>
#include <windows.h>
#include <iostream>
#include <cstdlib>
#include <fstream>
#include <stdlib.h>

#include <string>

using namespace std;

enum Direction { None, Up, Right, Down, Left };

CONSOLE_SCREEN_BUFFER_INFO csbi;
HANDLE hStdOutput;
BOOL bUsePause;

HANDLE FrontBuffer, BackBuffer;
PCHAR_INFO emptyBoard, activeBoard;
short rows, cols;
COORD foodPosition;
COORD wormHeadPosition;
COORD *wormPositions;
int nrOfBites;

void WriteFrame(PCHAR_INFO board)
{
	COORD boardSize = csbi.dwSize;

	COORD boardPosition;
	boardPosition.X = 0;
	boardPosition.Y = 0;

	SMALL_RECT srctWriteRect;
	srctWriteRect.Top = 0;
    srctWriteRect.Bottom = rows;
    srctWriteRect.Left = 0;
    srctWriteRect.Right = cols;

	BOOL result = WriteConsoleOutput(FrontBuffer,		// screen buffer to write to 
									board,				// buffer to copy from 
									boardSize,			// col-row size of chiBuffer 
									boardPosition,		// top left src cell in the buffer 
									&srctWriteRect);	// dest. screen buffer rectangle 
}

void SwitchBuffers(void)
{
	HANDLE temp = FrontBuffer;
	FrontBuffer = BackBuffer;
	BackBuffer = temp;
}

void PutWormInActiveBuffer()
{
	int xPos = wormHeadPosition.X;
	int yPos = (wormHeadPosition.Y * cols);
	activeBoard[yPos + xPos].Char.UnicodeChar = '@';

	for(int i = 0; i < nrOfBites; i++)
	{
		xPos = wormPositions[i].X;
		yPos = (wormPositions[i].Y * cols);
		activeBoard[yPos + xPos].Char.UnicodeChar = '.';
	}
}

void PutFoodInActiveBuffer()
{
	int yPos = (foodPosition.Y * cols);
	int xPos = foodPosition.X;
	activeBoard[yPos + xPos].Char.UnicodeChar = '*';
}

void SetWormPosition(Direction direction)
{
	for(int i = nrOfBites; i > 0; i--)
	{
		wormPositions[i].X = wormPositions[i-1].X;
		wormPositions[i].Y = wormPositions[i-1].Y;
	}
	wormPositions[0].X = wormHeadPosition.X;
	wormPositions[0].Y = wormHeadPosition.Y;

	if(direction == Right)
		wormHeadPosition.X++;
	if(direction == Left)
		wormHeadPosition.X--;
	if(direction == Down)
		wormHeadPosition.Y++;
	if(direction == Up)
		wormHeadPosition.Y--;
}

bool CollisionDetection(void)
{
	bool outOfBounds = wormHeadPosition.X <= 0 
					|| wormHeadPosition.X >= cols - 1
					|| wormHeadPosition.Y <= 0
					|| wormHeadPosition.Y >= rows - 1;

	int xPos, yPos;
	for(int i = 0; i < nrOfBites; i++)
	{
		xPos = wormPositions[i].X;
		yPos = wormPositions[i].Y;
		if(wormHeadPosition.X == xPos && wormHeadPosition.Y == yPos)
			outOfBounds = true;
	}

	return outOfBounds;
}

bool FoodDetection(void)
{
	return (wormHeadPosition.X == foodPosition.X && wormHeadPosition.Y == foodPosition.Y);
}

void GenerateFoodPosition(void)
{
	foodPosition.X = 1 + rand() % (cols-2);
	foodPosition.Y = 1 + rand() % (rows-2);
}

void CreateBoard(PCHAR_INFO board)
{
	for(int row = 0; row < rows; row++)
	{
		for(int col = 0; col < cols; col++)
		{
			if(row == 0 || row == rows-1 || col == 0 || col == cols-1)
				board[row*cols+col].Char.UnicodeChar = '#';	// wall
			else
				board[row*cols+col].Char.UnicodeChar = ' ';
		}
	}

	for(int i = 0; i < (cols*rows); i++)
		board[i].Attributes = 7;
}

void CopyBoard(PCHAR_INFO source, PCHAR_INFO target)
{
	for(int i = 0; i < (cols*rows); i++)
	{
		target[i].Attributes = source[i].Attributes;
		target[i].Char.UnicodeChar = source[i].Char.UnicodeChar;
	}
}

bool MoveInDirection(Direction direction)
{
	// put empty board in buffer
	CopyBoard(emptyBoard, activeBoard);

	// move worm one step in the current direction
	SetWormPosition(direction);

	if(FoodDetection())
	{
		nrOfBites++;
		GenerateFoodPosition();
	}

	if(CollisionDetection())
		return false;

	PutWormInActiveBuffer();

	PutFoodInActiveBuffer();

	// print buffer
	WriteFrame(activeBoard);
	BOOL result = SetConsoleActiveScreenBuffer(FrontBuffer);
	SwitchBuffers();

	return true;
}

Direction GetDirection(int keyChar, Direction currentValue)
{
	switch(keyChar)
	{
		case 72:
			if(currentValue == Down)
				return currentValue;
			return Up;
		case 77:
			if(currentValue == Left)
				return currentValue;
			return Right;
		case 80:
			if(currentValue == Up)
				return currentValue;
			return Down;
		case 75:
			if(currentValue == Right)
				return currentValue;
			return Left;
		default:
			return currentValue;
	}
}

void GameOn()
{
	CreateBoard(emptyBoard);

	wormHeadPosition.X = cols/2;
	wormHeadPosition.Y = rows/2;

	GenerateFoodPosition();

	nrOfBites = 0;

	BOOL result = SetConsoleActiveScreenBuffer(FrontBuffer);

	int keyChar;
	Direction d = Right;
	while(1)
	{
		if(_kbhit())
		{
			keyChar = _getch();
			d = GetDirection(keyChar, d);
		}

		if(!MoveInDirection(d))
		{
			break;
		}
		
		Sleep(100);
	}
}

void WriteTextMessageToBuffer(std::wstring message, int yOffset)
{
	int messageLength = message.length();
	int yPos = rows/2 - yOffset;
	int xPos = cols/2 - messageLength/2;
	int offset = (yPos*cols) + xPos;
	const wchar_t *p = message.c_str();
	for(int i = 0; i < messageLength; i++)
		activeBoard[offset + i].Char.UnicodeChar = WCHAR(p[i]);
}

bool GameOver(void)
{
	CopyBoard(emptyBoard, activeBoard);

	std::wstring score(L"Score: ");
	score += std::to_wstring(nrOfBites);

	WriteTextMessageToBuffer(std::wstring(L"GAME OVER"), 2);
	WriteTextMessageToBuffer(score, 0);
	WriteTextMessageToBuffer(std::wstring(L"Play again? (y/n)"), -2);

	WriteFrame(activeBoard);
	BOOL result = SetConsoleActiveScreenBuffer(FrontBuffer);

	while(1)
	{
		if(_kbhit())
		{
			int ch = _getch();
			if(ch == 'y')
				return true;
			if(ch == 'n')
				return false;
		}
		Sleep(100);
	}
}

void main(void)
{
	hStdOutput = GetStdHandle(STD_OUTPUT_HANDLE);

	if (!GetConsoleScreenBufferInfo(hStdOutput, &csbi))
	{
		printf("GetConsoleScreenBufferInfo failed: %d\n", GetLastError());
		return;
	}

	FrontBuffer = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
	BackBuffer = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);

	rows = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
	cols = csbi.srWindow.Right - csbi.srWindow.Left + 1;

	bool replay = true;
	while(replay)
	{
		//char* gameBoard = new char[rows * cols];

		// CHAR_INFO gameBoard[2000]; // [20][80]; 
		emptyBoard = new CHAR_INFO[rows * cols];
		activeBoard = new CHAR_INFO[rows * cols];
		wormPositions = new COORD[rows * cols];

		GameOn();

		replay = GameOver();

		delete [] emptyBoard;
		delete [] activeBoard;
		delete [] wormPositions;
	}
}
