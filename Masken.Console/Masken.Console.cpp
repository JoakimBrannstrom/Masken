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

CONSOLE_SCREEN_BUFFER_INFO csbi;
HANDLE hStdOutput;
BOOL bUsePause;

HANDLE FrontBuffer, BackBuffer;
PCHAR_INFO emptyBoard, activeBoard;
short rows, cols;
COORD wormPosition;

void CreateBoard(PCHAR_INFO board)
{
	for(int i = 0; i < cols; i++)
		board[i].Char.UnicodeChar = '#';

	for(int row = 1; row < rows; row++)
		for(int col = 0; col < cols; col++)
		{
			if(col == 0 || col == cols-1)
				board[row*cols+col].Char.UnicodeChar = '#';
			else
				board[row*cols+col].Char.UnicodeChar = ' ';
		}

	for(int i = (cols*(rows-1)); i < (cols*rows); i++)
		board[i].Char.UnicodeChar = '#';

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
									boardPosition,		// top left src cell in chiBuffer 
									&srctWriteRect);	// dest. screen buffer rectangle 
}

void SwitchBuffers(void)
{
	HANDLE temp = FrontBuffer;
	FrontBuffer = BackBuffer;
	BackBuffer = temp;
}

enum Direction { None, Up, Right, Down, Left };

void SetWormPosition(Direction direction)
{
	if(direction == Right)
		wormPosition.X++;
	if(direction == Left)
		wormPosition.X--;
	if(direction == Down)
		wormPosition.Y++;
	if(direction == Up)
		wormPosition.Y--;

	if(wormPosition.X >= cols-1)
		wormPosition.X == cols/2;

	if(wormPosition.Y >= rows-1)
		wormPosition.Y == rows/2;
}

void PutWormInActiveBuffer()
{
	activeBoard[(wormPosition.Y * cols) + wormPosition.X].Char.UnicodeChar = '@';
}

void MoveInDirection(Direction direction)
{
	// put gameBoard in buffer
	CopyBoard(emptyBoard, activeBoard);

	// move worm one step in direction
	SetWormPosition(direction);

	// put worm in buffer
	PutWormInActiveBuffer();

	// print buffer
	WriteFrame(activeBoard);
	BOOL result = SetConsoleActiveScreenBuffer(FrontBuffer);
	SwitchBuffers();
}

Direction GetDirection(int keyChar, Direction defaultValue)
{
	switch(keyChar)
	{
		case 72:
			return Up;
		case 77:
			return Right;
		case 80:
			return Down;
		case 75:
			return Left;
		default:
			return defaultValue;
	}
}

void GameOn()
{
	//char* gameBoard = new char[rows * cols];

    // CHAR_INFO gameBoard[2000]; // [20][80]; 
    emptyBoard = new CHAR_INFO[rows * cols];
    activeBoard = new CHAR_INFO[rows * cols];
	CreateBoard(emptyBoard);

	wormPosition.X = cols/2;
	wormPosition.Y = rows/2;

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

		MoveInDirection(d);
		
		Sleep(100);
	}

	delete [] emptyBoard;
	delete [] activeBoard;
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

	GameOn();

	printf("\n\tPress any key to exit...\n");
	int ch = _getch();
}
