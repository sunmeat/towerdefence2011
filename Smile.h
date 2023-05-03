#pragma once
#include "Point.h"

class Smile
{
	Point P1;
	Point P2;
	Point P3;

public:

	Smile(void)
	{
		P1 = Point (0,0,'*',15);
		P2 = Point (0,2,'*',15);
		P3 = Point (1,1,'*',15);
	}

	Smile(int x, int y)
	{
		P1 = Point (0+x, 0+y, '*',15);
		P2 = Point (0+x, 2+y, '*',15);
		P3 = Point (1+x, 1+y, '*',15);
	}

	void Right (int i) 
	{
		P1.Right(i);
		P2.Right(i);
		P3.Right(i);

	}
	~Smile(void)
	{
	}

	void Show () {
		P1.Show();
		P2.Show();
		P3.Show();
	}
};

