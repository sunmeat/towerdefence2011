#pragma once
#include "Unit.h"
#include <iostream>
using namespace std;

class Tower: public Unit
{

public:

	static int money;
	static int countTower;
	static int lost;
	int cost ; // цена юнита
	static bool tobuy;
	static int numbuy;
	static bool select;


	int bullets; // количество патронов
	Tower() : Unit ( )
	{
		char digit [3] = {str[17], str[18], '\0'};
		bullets = atoi (digit);

		digit[0] = str[20];
		digit[1] = str[21];
		cost = atoi (digit);
	}

	Tower(Data unit, int state, int X, int Y) : Unit ( unit, state, X, Y)
	{
		char digit [3] = {str[17], str[18], '\0'};
		bullets = atoi (digit);

		digit[0] = str[20];
		digit[1] = str[21];
		cost = atoi (digit);
	}

	Tower(const Tower & ob) : Unit (ob)
	{
		bullets = ob.bullets;
		cost = ob.cost;
	}

	void Reset (int _X, int _Y)
	{
		char digit [3] = {str[17], str[18], '\0'};
		bullets = atoi (digit);
		damage = 3;
		state = 4;
		deleted = false;
		X = _X;
		Y = _Y;
	}

	void ReDraw ()
	{
		if (fire && state > 3 && bullets && charge == 3) bullets --;
		if (bullets == 0) 
		{
			ShellExecuteA(NULL,"open","wavplay.exe", "nobullets.wav", NULL, 0) ;
			bullets = -1;
		}

		size = 0;
		if (m) delete [] m;
		m = nullptr;
		Draw();

		if (state > 3 )
		{
			if (bullets > 0)
				Scr.ChangeInt (bullets , GetX() + 4, GetY() - 1, 11);
			else Scr.ChangeStr ("No bullets", GetX(), GetY() - 1, 12);
		}

	}

	void  BuyBullets(int i)
	{
		if (i > money && state >3 ) 
			ShellExecuteA(NULL,"open","wavplay.exe", "nomoney.wav", NULL, 0) ;
		else if (state >3)
		{
			if ( bullets == -1) bullets ++; // выровняли -1 т.к. было для блокировки повторного звука нет патронов
			bullets += i;  
			ShellExecuteA(NULL,"open","wavplay.exe", "buybullets.wav", NULL, 0) ;
			Tower :: money -= 10;
		}
	}


	~Tower(void)
	{
	}
};

int Tower :: money;
int  Tower :: countTower;
int Tower :: lost;
bool Tower::tobuy = 0;
int Tower:: numbuy;
bool Tower::select = 1;

