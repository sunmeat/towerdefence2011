#pragma once
#include "Point.h"

class Object 
{
protected:	
	int color ; //���� 
	int colorExt; //  ���� ���. �������
	char ext; // ���. ������
	int * index; //������ �������� � ������� ����� ������� ������ �����
	int X, Y;   //��������� ����� ������� ����������
	char * str; //��������� �� ������ �� �������� � ��������������
	Point * m; //������ �����
	int size ; // ������ ������� �����
	bool changeState; //���� � ����� ������

public:
	bool deleted; //������� �� ��������
	int layer; //���� �� ���� � ����, � ���� ����, ��� �� ������ ����
	int state; // ���������� ����� ����������� �� ������� �� �����

	void SetState(int _state) {	state = _state;	}

	void Draw (int _state) //��� ������� �������� ���� Draw
	{
		if (state != _state || m == nullptr) 
		{
			state = _state;
			size = 0;
			for (int i = index[state]; str [i] != '~' ; i++) 
				if ( str[i] !='\n')
					size ++; //���������� �����
			if (m) delete [] m;
			m = new Point [size] ;

			int x = 0; 
			int y = 0;
			int count = 0; // ������� �����
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
					m[count].SetColor(colorExt); // ���. ������ 

				count++;
				x++;
			}
		}

		for (int i = 0; i < size; i++) 
			m[i].Show();
	} 

		Object(Data object, int _state, int _X, int _Y) // �������� ���������, ���������� � ������ � ���������� �������
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

	Object() // �������� ���������, ���������� � ������ � ���������� �������
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


