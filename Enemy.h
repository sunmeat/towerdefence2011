#pragma once
#include "Unit.h"
#include "iostream"
using namespace std;

enum {MoveRight, MoveDown, MoveLeft, MoveUp };


class Enemy: public Unit
{ 

public:

	static int countEnemy;
	static int kills;
	int speed; //скорость движения 0 - 9, 0 - максимальное, т.е это число пропусков своего шага на одну клетку в общем цикле движения
	int direction; // направление движения
	int killmoney; // деньги за смерть
	int moves ; //число клеток перемещений
	
	Enemy(Data unit, int state, int X, int Y, int _direction = MoveRight ) : Unit (unit, state, X, Y)
	{
		direction = _direction;
		moves = 0;
		char digit [3] = {str[17], str[18], '\0'};
		speed = atoi (digit);

		digit[0] = str[20];
		digit[1] = str[21];
		killmoney = atoi (digit);
	}

	void Move ()
	{
		if ( state > 3 && takt % (speed) == 0)
		{ 
			moves ++;
			switch (direction)
			{
			case (MoveRight):
				X ++ ;
				break;

			case (MoveLeft):
				X -- ;
				break;

			case (MoveUp):
				Y -- ;
				break;

			case (MoveDown):
				Y ++ ;
				break;
			}
		}
		size = 0;
		if (m) delete [] m;
		m = nullptr;
		Draw();
		if (state == 0 && !deleted) 
			Scr.ChangeInt (killmoney , GetX() + 4, GetY(), 14);
	}

	void Reset (int _X, int _Y,  int _direction) 
	{
		X = _X;
		Y = _Y;
		direction = _direction;
		state = 4;
		deleted = false;
		moves = 0;
		damage = 3;
	}

	~Enemy(void) 
	{
	}
};

int Enemy::countEnemy;
int Enemy::kills;