#include "font.h";
#include "PhilipsPCD8544.h";

#define LCD_COMMAND	LOW
#define LCD_DATA	HIGH

void InitializeLcd()
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

short clearDisplay(void)
{
	int nrOfPixels = 84 * 48; // x * y
	int nrOfCharacterPositions = nrOfPixels / 8; // 84 * 48 / 8 = 84 * 6 = 504
	for(int i = 0; i < 504; i++)
		LcdWriteData(0x00); // clear LCD
}

short getDisplayWidth(void)
{
	return 84;
}

short getDisplayHeight(void)
{
	return 48;
}

void drawFrame(void)
{
	unsigned char  j;

	for(j = 0; j < 84; j++) // Top
	{
		LcdXY(j, 0);
		LcdWrite(LCD_DATA, 0x01);
	}

	for(j = 0; j < 84; j++) // Bottom
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

void RawWriteDisplay(bool *pixelBuffer)
{
	short width = getDisplayWidth();
	short height = getDisplayHeight();
	int displaySize = width * height;
	int x, y;

	for(int i = 0; i < displaySize; i++)
	{
		x = i % width;
		y = i % height;
		LcdXY(x, y);
		if(pixelBuffer[i])
			LcdWrite(1, 0x01);
		else
			LcdWrite(0, 0x01);
	}
}

void LcdWriteString(char *characters)
{
	while(*characters)
		LcdWriteCharacter(*characters++);
}

void LcdWriteCharacter(char character)
{
	for(int i=0; i<5; i++)
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
