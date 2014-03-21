#include "types.h";
#include "PhilipsPCD8544.h";

short rows, cols;
bool gameOver = false;

COORD foodPosition, wormHeadPosition;
COORD wormPositions[504];
int nrOfBites;
Direction d;

void setup()
{
	rows = getDisplayHeight();
	cols = getDisplayWidth();

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

	InitializeLcd();

	//LcdXY(4, 2);
	//LcdWriteString("Starting...");

	InitializeGame();
}

void loop()
{
/*
	if(replay)
	{
		GameOn();

		//GameOver();
	}
	*/
	if(gameOver || !MoveInDirection(d))
	{
		gameOver = true;
		GameOver();
	}
		
	delay(200);
}

void InitializeGame()
{
	WriteEmptyBoard();

	wormHeadPosition.X = cols/2;
	wormHeadPosition.Y = rows/8/2-1;

	GenerateFoodPosition();

	nrOfBites = 5;

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
	drawFrame();
}

void GenerateFoodPosition(void)
{
	foodPosition.X = 1 + rand() % (cols-2);
	foodPosition.Y = 1 + rand() % (rows-2);
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
		GenerateFoodPosition();
		PutFoodInActiveBuffer();
	}
	else
		SetWormTailPosition(previousPosition);

	if(CollisionDetection())
		return false;

	return true;
}

void CopyBoard(bool *source, bool *target)
{
	for(int i = 0; i < (cols*rows); i++)
	{
		target[i] = source[i];
	}
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
	LcdXY(xPos, yPos);
	LcdWriteData(0x01);

	return previousPosition;
}

void SetWormTailPosition(COORD previousPosition)
{
	LcdXY(wormPositions[nrOfBites].X, wormPositions[nrOfBites].Y);
	LcdWriteData(0x00);

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

void PutFoodInActiveBuffer()
{
//	int yPos = (foodPosition.Y * cols);
	//int xPos = foodPosition.X;
	//activeBoard[yPos + xPos] = true;

	int xPos = foodPosition.X;
	int yPos = foodPosition.Y;
	LcdXY(xPos, yPos);
	LcdWriteData(0x03);
	LcdXY(xPos+1, yPos);
	LcdWriteData(0x03);
}

void WriteFrame(bool *board)
{
	RawWriteDisplay(board);
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
}
