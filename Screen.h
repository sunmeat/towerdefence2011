#pragma once
HANDLE H = GetStdHandle(-11);
Pixel Pix;
COORD cursorPos;
DWORD cWritten;
WORD wColors;

class Screen
{
public:
	int lines , cols ;
	Pixel ** m;

	Screen(int _cols, int _lines )
	{
		cols =_cols - 25;
		lines =_lines;
		m = new Pixel * [cols];
		for (int i = 0; i < cols; i++)
			m[i] = new Pixel [lines];

		for (int i = 0; i < cols; i++)
			for (int j = 0; j < lines; j++)
			{
				m[i][j].changed = true;
				m[i][j].layer = 0;
				m[i][j].symbol = ' ';
				m[i][j].color = 12;
			}
	}

	void ChangePixel(Pixel data)
	{
		int i = data.x;
		int j = data.y;
		if (data.layer >= m[i][j].layer && m[i][j].symbol != data.symbol)
		{
			if (data.symbol == '_') data.symbol = ' ';
			m[i][j].color = data.color;
			m[i][j].symbol = data.symbol;
			m[i][j].changed = true;
			m[i][j].layer = data.layer;
		}
		else 
			m[i][j].changed = false;

	}

	void ChangeStr(char * str, int x, int y , int color)
	{
		Pix.x = x;
		Pix.y = y;
		Pix.color = color;

		for (int i = 0; i < strlen(str); i++)
		{
			Pix.symbol = str[i];
			ChangePixel (Pix);
			Pix.x ++;
		}
	}

	void ChangeInt (int val, int x, int y, int color)
	{
		char str [10];
		itoa (val, str, 10);
		Pix.x = x;
		Pix.y = y;
		Pix.color = color;
		for (int i = 0; i < strlen(str); i++)
		{
			Pix.symbol = str[i];
			ChangePixel (Pix);
			Pix.x ++;
		}
	}

	void Draw () 
	{
		for (int i = 0; i < cols; i++)
			for (int j = 0; j < lines; j++)
				if (m[i][j].changed)
				{
					cursorPos.X = i; 
					cursorPos.Y = j;
					SetConsoleCursorPosition(H, cursorPos);
					SetConsoleTextAttribute(H, m[i][j].color);
					putchar (m[i][j].symbol);
					
					//WORD wColors = m[i][j].color;
					//WriteConsoleOutputCharacter(H, &m[i][j].symbol, 1, cursorPos, &cWritten);
					//WriteConsoleOutputAttribute(H, &wColors, 1, cursorPos, &cWritten);

					m[i][j].changed = false;
				}
	}

	~Screen(void)
	{
		for (int i = 0; i < lines; i++)
			delete [] m[i];
		delete [] m;
	}
};

