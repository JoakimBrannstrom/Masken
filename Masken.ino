#include "types.h";
#include "PhilipsPCD8544.h";

bool gameOver = false;

COORD foodPosition, wormHeadPosition;
COORD wormPositions[204];

short bufferPosition = 0;
byte screenBuffer[DisplaySizeInBytes];

int nrOfBites;
Direction d;
short state = 0;

void setup()
{
	//char* gameBoard = new char[rows * cols];
	// CHAR_INFO gameBoard[2000]; // [20][80];
	/*
	emptyBoard = new bool[rows * cols];
	activeBoard = new bool[rows * cols];
	wormPositions = new COORD[rows * cols];
	*/

	//LcdXY(20,2);
	//LcdWriteString("THE END!");
	//drawFrame();

	initializeLcd();

	//LcdXY(4, 2);
	//LcdWriteString("Starting...");

	delay(80);
	InitializeGame();
}

void loop()
{
	autoPlay();

	if(gameOver || !MoveInDirection(d))
	{
		gameOver = true;
		GameOver();
		delay(1000);
	}
		
	//delay(100);
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

void InitializeGame()
{
	WriteEmptyBoard();

	wormHeadPosition.X = 1;
	wormHeadPosition.Y = 1;

	GenerateFoodPosition();

	nrOfBites = 10;

	d = Right;
}

void GameOn()
{
	while(1)
	{
		/*
		if(_kbhit())
		{
			keyChar = _getch();
			d = GetDirection(keyChar, d);
		}
		*/
		if(!MoveInDirection(d))
		{
			break;
		}
		
		delay(100);
	}
}
/*
void CreateBoard(bool *board)
{
	for(int row = 0; row < rows; row++)
	{
		for(int col = 0; col < cols; col++)
		{
			if(row == 0 || row == rows-1 || col == 0 || col == cols-1)
				board[row*cols+col] = true;	// wall
			else
				board[row*cols+col] = false;
		}
	}
}
*/

void WriteEmptyBoard()
{
	unsigned char j;

	for(j = 0; j < DisplayWidth; j++) // Top
	{
		bufferXY(j, 0);
		bufferWriteData(0x01);
	}

	for(j = 0; j < DisplayWidth; j++) // Bottom
	{
		bufferXY(j, 5);
		bufferWriteData(0x80);
	}

	for(j = 0; j < 6; j++) // Right
	{
		bufferXY(83, j);
		bufferWriteData(0xff);
	}

	for(j = 0; j < 6; j++) // Left
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

bool MoveInDirection(Direction direction)
{
	// move worm one step in the current direction
	COORD previousPosition = SetWormHeadPosition(direction);

	if(FoodDetection())
	{
		nrOfBites++;
		//clearBuffer(foodPosition.X, foodPosition.Y);
		GenerateFoodPosition();
		// PutFoodInActiveBuffer();
	}

	SetWormTailPosition(previousPosition);

	PutFoodInActiveBuffer();

	sendBufferToLcd();

	if(CollisionDetection())
		return false;

	return true;
}

COORD SetWormHeadPosition(Direction direction)
{
	COORD previousPosition;
	previousPosition.X = wormHeadPosition.X;
	previousPosition.Y = wormHeadPosition.Y;

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
	//LcdXY(xPos, yPos);
	//LcdWriteData(0x01);
	drawBuffer(xPos, yPos);

	return previousPosition;
}

void SetWormTailPosition(COORD previousPosition)
{
/*
	LcdXY(wormPositions[nrOfBites].X, wormPositions[nrOfBites].Y);
	LcdWriteData(0x00);
*/
//	bufferXY(wormPositions[nrOfBites].X, wormPositions[nrOfBites].Y);
	//bufferWriteData(0x00);
	clearBuffer(wormPositions[nrOfBites].X, wormPositions[nrOfBites].Y);

	for(int i = nrOfBites; i > 0; i--)
	{
		wormPositions[i].X = wormPositions[i-1].X;
		wormPositions[i].Y = wormPositions[i-1].Y;
	}
	wormPositions[0].X = previousPosition.X;
	wormPositions[0].Y = previousPosition.Y;
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
/*
	LcdXY(xPos, yPos);
	LcdWriteData(0x03);
	LcdXY(xPos+1, yPos);
	LcdWriteData(0x03);
*/
	drawBuffer(xPos, yPos);
}

void GameOver(void)
{
	clearDisplay();

	LcdXY(16, 1);
	LcdWriteString("GAME OVER");

	LcdXY(16, 3);
	char score[15];
	sprintf(score, "Score: %d", nrOfBites);
	LcdWriteString(score);

	LcdXY(0, 5);
	char head[15];
	sprintf(head, "S:%d X:%d Y:%d", state, wormHeadPosition.X, wormHeadPosition.Y);
	LcdWriteString(head);
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
