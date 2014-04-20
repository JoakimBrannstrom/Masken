#include "types.h";
#include "PhilipsPCD8544.h";

#define MaxBites 225

bool gameOver = false;

COORD foodPosition, wormHeadPosition;
COORD wormPositions[MaxBites];

short bufferPosition = 0;
byte screenBuffer[DisplaySizeInBytes];

int nrOfBites;
Direction d;
short state = 0;

void setup()
{
	initializeLcd();

	delay(80);
	clearBuffer();
	clearDisplay();

	InitializeGame();
}

void loop()
{
	autoPlay();

	if(nrOfBites >= MaxBites)
	{
		LevelComplete();
		delay(1000);
	}
	else if(gameOver || !MoveInDirection(d))
	{
		gameOver = true;
		GameOver();
		delay(1000);
	}
}

void InitializeGame()
{
	WriteEmptyBoard();

	wormHeadPosition.X = 1;
	wormHeadPosition.Y = 1;

	GenerateFoodPosition();

	nrOfBites = 5;

	d = Right;
}

void WriteEmptyBoard()
{
	unsigned char j;

	int displayHeightInBytes = (DisplayHeight/8);
	if(displayHeightInBytes < 2)
	{
		for(j = 0; j < DisplayWidth; j++) // Top & Bottom
		{
			bufferXY(j, 0);
			bufferWriteData(0x81);
		}
	}
	else
	{
		for(j = 0; j < DisplayWidth; j++) // Top
		{
			bufferXY(j, 0);
			bufferWriteData(0x01);
		}

		for(j = 0; j < DisplayWidth; j++) // Bottom
		{
			bufferXY(j, displayHeightInBytes-1);
			bufferWriteData(0x80);
		}
	}
	
	for(j = 0; j < displayHeightInBytes; j++) // Right
	{
		bufferXY(DisplayWidth-1, j);
		bufferWriteData(0xff);
	}

	for(j = 0; j < displayHeightInBytes; j++) // Left
	{
		bufferXY(0, j);
		bufferWriteData(0xff);
	}
}

void GenerateFoodPosition(void)
{
	srand(millis());
	foodPosition.X = 1 + rand() % (DisplayWidth-2);
	foodPosition.Y = 1 + rand() % (DisplayHeight-2);
}

void autoPlay()
{
	if(state == 0 && wormHeadPosition.X > DisplayWidth-3)
	{
		state = 1;
		d = Down;
	}
	else if(state == 1 && wormHeadPosition.X > DisplayWidth-3)
	{
		state = 2;
		d = Left;
	}
	else if(state == 2 && wormHeadPosition.X < 3)
	{
		state = 3;
		d = Down;
	}
	else if(state == 3 && wormHeadPosition.X < 3)
	{
		state = 0;
		d = Right;
	}

	if((state == 3 || state == -1) && wormHeadPosition.Y > DisplayHeight-4)
	{
		state = -1;
		if(wormHeadPosition.X < 2)
			d = Up;
		else
			d = Left;
	}

	if(state == -1 && wormHeadPosition.Y < 2)
	{
		state = 0;
		d = Right;
	}
}

bool MoveInDirection(Direction direction)
{
	// move worm one step in the current direction
	COORD neckPosition = SetWormHeadPosition(direction);

	if(FoodDetection())
	{
		nrOfBites++;
		GenerateFoodPosition();
	}

	SetWormTailPosition(neckPosition);

	PutFoodInActiveBuffer();

	sendBufferToLcd();
	delay(10);

	if(CollisionDetection())
		return false;

	return true;
}

COORD SetWormHeadPosition(Direction direction)
{
	COORD neckPosition;
	neckPosition.X = wormHeadPosition.X;
	neckPosition.Y = wormHeadPosition.Y;

	if(direction == Right)
		wormHeadPosition.X++;
	if(direction == Left)
		wormHeadPosition.X--;
	if(direction == Down)
		wormHeadPosition.Y++;
	if(direction == Up)
		wormHeadPosition.Y--;

	int xPos = wormHeadPosition.X;
	int yPos = wormHeadPosition.Y;

	drawBuffer(xPos, yPos);

	return neckPosition;
}

void SetWormTailPosition(COORD neckPosition)
{
	clearBuffer(wormPositions[nrOfBites].X, wormPositions[nrOfBites].Y);

	for(int i = nrOfBites; i > 0; i--)
	{
		wormPositions[i].X = wormPositions[i-1].X;
		wormPositions[i].Y = wormPositions[i-1].Y;
	}
	wormPositions[0].X = neckPosition.X;
	wormPositions[0].Y = neckPosition.Y;
}

bool FoodDetection(void)
{
	return (wormHeadPosition.X == foodPosition.X && wormHeadPosition.Y == foodPosition.Y);
}

bool CollisionDetection(void)
{
	bool outOfBounds = wormHeadPosition.X <= 0 
					|| wormHeadPosition.X >= DisplayWidth - 1
					|| wormHeadPosition.Y <= 0
					|| wormHeadPosition.Y >= DisplayHeight - 1;

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

void PutFoodInActiveBuffer()
{
	int xPos = foodPosition.X;
	int yPos = foodPosition.Y;

	drawBuffer(xPos, yPos);
}

void LevelComplete(void)
{
	clearDisplay();

	LcdXY(0, 1);
	LcdWriteString("Level complete");

	LcdXY(10, 3);
	char score[15];
	sprintf(score, "Score: %d", nrOfBites);
	LcdWriteString(score);
}

void GameOver(void)
{
	clearDisplay();

	LcdXY(16, 1);
	LcdWriteString("GAME OVER");

	LcdXY(12, 3);
	char score[15];
	sprintf(score, "Score: %d", nrOfBites);
	LcdWriteString(score);
/*
	// Debug info:
	LcdXY(0, 5);
	char head[15];
	sprintf(head, "S:%d X:%d Y:%d", state, wormHeadPosition.X, wormHeadPosition.Y);
	LcdWriteString(head);
*/
}

void drawBuffer(short x, short y)
{
	const int byteSize = 8;
	short bigY = y / byteSize;
	short smallY = y % byteSize;

	bufferXY(x, bigY);

	byte pattern = 0x01 << smallY;
	byte data = screenBuffer[bufferPosition];
	screenBuffer[bufferPosition] = data | pattern;
}

void clearBuffer(short x, short y)
{
	const int byteSize = 8;
	short bigY = y / byteSize;
	short smallY = y % byteSize;

	bufferXY(x, bigY);

	byte pattern = 0x01 << smallY;
	byte data = screenBuffer[bufferPosition];
	screenBuffer[bufferPosition] = data & (pattern ^ 0xff);
}

void bufferXY(short x, short y)
{
	bufferPosition = x + (y * DisplayWidth);
}

void bufferWriteData(byte data)
{
	if(0 <= bufferPosition && bufferPosition < DisplaySizeInBytes)
	{
		screenBuffer[bufferPosition] = data;
		bufferPosition++;
	}
}

void sendBufferToLcd()
{
	LcdXY(0, 0);
	
	for(int i = 0; i < DisplaySizeInBytes; i++)
		LcdWriteData(screenBuffer[i]);
}

void clearBuffer()
{
	bufferPosition = 0;
	for(int i = 0; i < DisplaySizeInBytes; i++)
		screenBuffer[i] = 0x00;
}
