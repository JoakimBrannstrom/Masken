#include "PhilipsPCD8544.h";

#define LCD_COMMAND	LOW
#define LCD_DATA	HIGH

void initializeLcd()
{
	pinMode(RST, OUTPUT);
	pinMode(CE, OUTPUT);
	pinMode(DC, OUTPUT);
	pinMode(DIN, OUTPUT);
	pinMode(CLK, OUTPUT);

	digitalWrite(RST, LOW);
	digitalWrite(RST, HIGH);

	LcdWriteCommand(0x21); // LCD extended commands
	LcdWriteCommand(0xB8); // set LCD Vop (contrast)
	// LcdWriteCommand(0xB4); // set LCD Vop (contrast)
	LcdWriteCommand(0x04); // set temp coefficent
	LcdWriteCommand(0x14); // LCD bias mode 1:40
	LcdWriteCommand(0x20); // LCD basic commands
	// LcdWriteCommand(0x09); // LCD all segments on
	LcdWriteCommand(0x0C); // LCD normal video

	clearDisplay();
}

void clearDisplay(void)
{
	for(int i = 0; i < DisplaySizeInBytes; i++)
		LcdWriteData(0x00); // clear LCD
}

void drawFrame(void)
{
	unsigned char j;

	for(j = 0; j < DisplayWidth; j++) // Top
	{
		LcdXY(j, 0);
		LcdWrite(LCD_DATA, 0x01);
	}

	for(j = 0; j < DisplayWidth; j++) // Bottom
	{
		LcdXY(j, 5);
		LcdWrite(LCD_DATA, 0x80);
	}

	for(j = 0; j < 6; j++) // Right
	{
		LcdXY(83, j);
		LcdWrite(LCD_DATA, 0xff);
	}

	for(j = 0; j < 6; j++) // Left
	{
		LcdXY(0, j);
		LcdWrite(LCD_DATA, 0xff);
	}
}

void drawBlock(short x, short y, short width, short height)
{
	short bigX = x / width;
	short smallX = x % width;
	const int byteSize = 8;
	short bigY = y / byteSize;
	short smallY = y % byteSize;

	LcdXY(bigX * width + smallX, bigY);
	long data = (0x0001 << height) - 1;
	long smallData = data << smallY;
	byte bigData = smallData >> 8;
	for(int i = smallX; i < smallX + width; i++)
	{
		LcdWriteData(smallData);
	}

	LcdXY(bigX * width + smallX, bigY + 1);
	for(int i = smallX; i < smallX + width; i++)
	{
		LcdWriteData(bigData);
	}
}

void LcdWriteString(char *characters)
{
	while(*characters)
		LcdWriteCharacter(*characters++);
}

void LcdWriteCharacter(char character)
{
	int i;
	for(i = 0; i < 5; i++)
		LcdWriteData(ASCII[character - 0x20][i]);

	LcdWriteData(0x00);
}

void LcdWriteData(byte data)
{
	LcdWrite(LCD_DATA, data);
}

void LcdWriteCommand(byte command)
{
	LcdWrite(LCD_COMMAND, command);
}

void LcdWrite(byte dc, byte value)
{
	digitalWrite(DC, dc); // DC pin is low for commands
	digitalWrite(CE, LOW);
	shiftOut(DIN, CLK, MSBFIRST, value); // Transmit serial data
	digitalWrite(CE, HIGH);
}

void LcdXY(int x, int y)
{
	LcdWriteCommand(0x80 | x);  // Column.
	LcdWriteCommand(0x40 | y);  // Row.
}
