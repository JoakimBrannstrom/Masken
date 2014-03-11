
#define Up		1
#define Right	2
#define Down	3
#define Left	4

int pinUp	= 1;
int pinRight= 2;
int pinDown	= 3;
int pinLeft	= 4;

int Direction;

int GetUserDirection();

void setup()
{
	Direction = Right;

	// Clear display

	// Set start position (top left)

	// Randomly put food on game board
}

void loop()
{
	int newDirection = GetUserDirection();
}

int GetUserDirection()
{
	if(digitalRead(pinUp))
		return Up;

	if(digitalRead(pinRight))
		return Right;

	if(digitalRead(pinDown))
		return Down;

	if(digitalRead(pinLeft))
		return Left;

	return 0;
}

void PrintBoard()
{
}

void PutFoodOnGameBoard()
{
}
