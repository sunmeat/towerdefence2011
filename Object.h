#pragma once
#include "Point.h"

class Object 
{
protected:	
	int color ; //цвет 
	int colorExt; //  цвет доп. значков
	char ext; // доп. значок
	int * index; //массив индексов с которых будет браться нужный слайд
	int X, Y;   //положение левой верхней координаты
	char * str; //указатель на данные со слайдами с псевдографикой
	Point * m; //массив точек
	int size ; // размер массива точек
	bool changeState; //флаг о смене слайда

public:
	bool deleted; //пометка на удаление
	int layer; //слой от нуля и выше, у кого ниже, тот на задний план
	int state; // порядковый номер указывающий на рисунок из файла

	void SetState(int _state) {	state = _state;	}

	void Draw (int _state) //для простых объектов своя Draw
	{
		if (state != _state || m == nullptr) 
		{
			state = _state;
			size = 0;
			for (int i = index[state]; str [i] != '~' ; i++) 
				if ( str[i] !='\n')
					size ++; //количество точек
			if (m) delete [] m;
			m = new Point [size] ;

			int x = 0; 
			int y = 0;
			int count = 0; // текущая точка
			for (int i = index[state]; str [i] != '~' ; i++) 
			{
				if(str[i] =='\n')
				{
					y++;
					x = 0;
					continue;
				}

				m[count].Set(X + x,Y + y, str[i], color) ;

				if (str[i] == ext) 
					m[count].SetColor(colorExt); // доп. значок 

				count++;
				x++;
			}
		}

		for (int i = 0; i < size; i++) 
			m[i].Show();
	} 

		Object(Data object, int _state, int _X, int _Y) // передаем состояние, координаты и данные о прорисовке объекта
	{
		layer = 0;
		deleted = false;
		m = nullptr;
		state = _state;
		size = 0;
		X = _X;
		Y = _Y;
		index = object.index;
		str = object.strData;

		color = atoi (str + 3);

		colorExt = atoi (str + 6);
		ext = str [9];

	}

	Object() // передаем состояние, координаты и данные о прорисовке объекта
	{
		layer = 0;
		deleted = false;
		m = nullptr;
		state = 4;
		size = 0;
		X = 0;
		Y = 0;
		index = 0;
		str = 0;

		color = atoi (str + 3);

		colorExt = atoi (str + 6);
		ext = str [9];

	}

	Object(const Object & ob)
	{
		layer = ob.layer;
		deleted = ob.deleted;
		m = ob.m;
		state = ob.state;
		size = ob.size;
		X = ob.X;
		Y = ob.Y;
		index = ob.index;
		str = ob.str;

		color = ob.color;

		colorExt = ob.colorExt;
		ext = ob.ext;
	}


	bool GetDel ()
	{
		return deleted;
	}

	int GetX()
	{
		return X;
	}

	int GetY()
	{
		return Y;
	}

	void SetLife ()
	{
		deleted = false;
	}

	
	~Object(void)
	{
	}
};


