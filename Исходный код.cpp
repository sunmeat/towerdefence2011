# define LEFT 75  // ��� ���������� �������� ��� �������
# define RIGHT 77
# define UP 72
# define DOWN 80

#define pathTuret "turet.txt" //��� �������� �����
#define pathTesla "tesla.txt" //��� �������� �����

#define pathEnemy "enemy.txt"
#define pathEnemy2 "enemy2.txt"
#define pathEnemy3 "enemy3.txt"

#define pathRoad "road.txt"

const int lines = 60; // ������� ����
const int cols = 150;

struct Data  //��������� ������ ��� ���������� ������� � ��� ���������������
{
	char* strData; //��������� �� ������, ����������� � ������ �� ������ �� ���� ����������(�������) �������
	int* index; //������� ������ ������ ������ ��� ������� ��������� �����
	int count; // ���������� ������� �������
	int size; // ������ ����� ������� ������ �� ��������
};

struct Pixel //��������� � ������� � ������ ("�������") ������������������ ������
{
	char symbol;
	int x, y;
	int layer;
	int color;
	bool changed;
};

#include "Object.h" // ����� ����� �������� ����-�� ������, ����� ��� ���� ��� ����� ������� ����� ���� ��������
#include "Unit.h"	// ����� ������ ���� �� ����� ��� ���������� ����
#include "Enemy.h"  // ����� ������ 
#include "Tower.h" //����� �����
#include "Screen.h" ///����� �������-��� �������� ��������� ������ �������� ������������� �������� ����������� ����� ����  �������� ������� ���������� �������������

#include <conio.h>
#include <vector>

using namespace std;

// ������� ��� �������� ������ �� �������� � ���������������� �������� �� ������
// ��������� ���� �� ���� �� �������� ������� � �� ������ � ����� ���������� ��������� Data �������� ���������� ������ �� �����
void LoadPics(char* str, Data& object)
{
	FILE* f = fopen(str, "r");
	if (!f) return; // ���� ����� ��� - ������
	fseek(f, 0, SEEK_END); // ���������� ������ � ����� �����
	object.size = ftell(f); // ����� ������ � ���������
	object.strData = new char[object.size]; // �������� ������ ��� ��� ���������� ��������
	rewind(f); // ��������� ������ � ������ �����
	fread(object.strData, sizeof(char), object.size, f);
	object.count = 0;
	for (int i = 0; i < object.size; i++) // 
		if (object.strData[i] == '$') object.count++;  //������� ������� �������
	object.index = new int[object.count]; // ������� ������ ��� �������� �������� ������� �������
	for (int k = 0, i = 0; i < object.size; i++) // 
		if (object.strData[i] == '$')
		{
			object.index[k] = i + 2; //��������� ������� � ������� ��������
			k++; // i+2 ������� ������ ��� i-�� ��� ��� $ � i+1 ��� ������ \r
		}
}

void color() //������ ������� ��� ���� ����� ���������� ���������� ����
{
	Point P1;
	for (int i = 0; i < 255; i++)
	{
		P1.SetColor(i);
		P1.Down(1);
		P1.Show();
		cout << i << endl;
	}
}

//������������� ���������� ������� �����
//��������� ���������� �������� ����� � ��������� ���������� � �������� ��������� event
DWORD WINAPI Mouse(void* p)
{
	DWORD click;
	while (1)
		ReadConsoleInput(M, (INPUT_RECORD*)p, 1, &click);
	return 0;
}

//������������� ���������� ���������� ���� ����� ������ ��� ������ ��� ��� ������� ������
DWORD WINAPI SreenDraw(void* p)
{
	while (1)
		Scr.Draw();
	return 0;
}

void main()
{
	char new_window_size[200];
	wsprintfA(new_window_size, "mode con cols=%d lines=%d", cols, lines);
	system(new_window_size);
	SetConsoleTitleA("Tower defense!");
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	GetConsoleScreenBufferInfo(H, &csbi); //���������� ������� ���� �������
	SetConsoleMode(M, ENABLE_MOUSE_INPUT | ENABLE_EXTENDED_FLAGS);

	CONSOLE_CURSOR_INFO cci;
	cci.bVisible = 0;
	cci.dwSize = 100;

	HWND hwnd = GetConsoleWindow();
	MoveWindow(hwnd, 35, 15, 1280, 800, true);

	SetConsoleCursorInfo(H, &cci);

	INPUT_RECORD event; //��� ����� �������� ��������� ���������� ������� �� ���� � ������ ��������

	Data turet;
	LoadPics(pathTuret, turet); //������ ��� ����� ����� 1
	Data tesla;
	LoadPics(pathTesla, tesla); //������ ��� ����� ����� 2


	Data Enemy1;
	LoadPics(pathEnemy, Enemy1); //������ ��� ����� ����� 1

	Data Enemy2;
	LoadPics(pathEnemy2, Enemy2); //������ ��� ����� ����� 1

	Data Enemy3;
	LoadPics(pathEnemy3, Enemy3); //������ ��� ����� ����� 1


	Data Road1;
	LoadPics(pathRoad, Road1); //������ ��� ������� ����� 1 - ��� ����� ���� ������� �����

	Unit::takt = 0; //������� ������ ����� ��������� ��������� ��������� ������ ������� (�����)

	const int speedcharge = 10; //����� �������� - ������ ����� ������ ��� ������ ��������� ����� ����������� ��� ���������� �������� � ����� ������

	vector <Tower> TV; //  ��������� ������ - ������� ��� �������� ������
	vector <Enemy> EV; //  

	Object R1(Road1, 0, 0, 0); //��� ����� � ��� ������� ������� ���� ������� ����� �����

	// ��������� ���������� ����� ������� ���� � ������������ ��������� ��������� ��� ������ �������� ��������
	//������ ���������� �������
	Unit::takt = 0;
	Tower::money = 200; //������ ������, �������� � ������ ����������
	Tower::countTower = 0; //������� ���������� �������������� �����
	Enemy::countEnemy = 0; // ������� ���������� �������������� ������
	Enemy::kills = 0; // ���������� ������� ������ �����
	Tower::lost = 0; // ���������� ������� ����� �����

	Enemy E1(Enemy1, 4, 0, 2, MoveRight);
	Enemy E2(Enemy2, 4, 0, 2, MoveRight);
	Enemy E3(Enemy3, 4, 0, 2, MoveRight);



	Tower Turet(turet, 4, cols - 15, 22); // ��� ����� ����������� ����� ������������� ����, � ������������ ����� 
	Tower Tesla(tesla, 4, cols - 15, 22); // ��� ����� ����������� ����� ������������� ����, � ������������ ����� 
	//Tower T2(Tower1, 4, 40, 20); // ��� ����� ����� ������� ����, 
	//��� ��������� ���� ����� � ������ ������ ���� �������  ������ ���� �� �������� ����� ����� ������ ����� ����

	//TV.push_back(T1);  //��� ����� ����������� ����� ��� ����� ������������� ���� �� ����� � ���� ��� ����� ������ ����� ������������� 

	//TV.push_back(T2);


	/////////////////////////////////////////////////////////////////////////////////

	// ������ ������ ������� ���������
	//CreateThread(0, 0, SreenDraw, &Scr, 0,0); //����������� ��������� ������ ������������� � ������� ( ����� � ��� �������� ������, �� ���������) 
	CreateThread(0, 0, Mouse, &event, 0, 0);  //���������� ������� ����

	//��������������� ���������� ��� ����� �������� ��������

	bool lb = 0; //����� ���� ����� ������ ������.
	bool rb = 0;//
	int taktclick = 0;
	const  int textShift = 23;
	const int panelShift = 22;
	const int towerWidth = 9;
	const int towerHigh = 7;
	const int space = 2;
	bool end = 0;
	bool win = 0;

	//int Moves [16] = {0,1,2,1,0,0,3,0,1,1,2,1,0,0,0,0} ; int index =0;

	int code; //��� ���������� ������� ������ �������������
	int tj; // "tower j " ���������� ��� ����������� ����������� ������ ����� � ������ ������� ����
	int ei; //���������� "enemy i"
	char str[20];
	COORD mousePos;

	//��������� ������ ������ �����		
	Tower turetPanel(turet, 4, cols - textShift, panelShift);
	turetPanel.DrawPanel();

	Tower teslaPanel(tesla, 4, cols - textShift + towerWidth + space, panelShift);
	teslaPanel.DrawPanel();

	// ���������� ��� ���� �������� �������� ������� ���� ������, ������ ������� ����

	while (1)
	{
		if (Unit::takt == 100000)  //����� ��� ������� ������ ������ ����� ��������� �����-�� ������������ ����� �����
			Unit::takt = 0;			//������ ��� �������� �� �������� ����� ����
		else Unit::takt++;

		if (Tower::money >= 1000)
		{
			end = true;
			win = true;
		}

		Scr.Draw(); //���������� ������, �� ��� ����� � ������� ��������� ������

		mousePos.X = event.Event.MouseEvent.dwMousePosition.X; // ������ ��������� �����
		mousePos.Y = event.Event.MouseEvent.dwMousePosition.Y;

		cursorPos.X = cols - textShift;
		cursorPos.Y = 7;
		SetConsoleCursorPosition(H, cursorPos);
		SetConsoleTextAttribute(H, 7);
		CharToOemA("���������: ", str);
		printf("%s%3d, %2d", str, mousePos.X, mousePos.Y);

		if (!taktclick && event.Event.MouseEvent.dwButtonState == FROM_LEFT_1ST_BUTTON_PRESSED)
		{
			lb = true;
			taktclick = 3;
		}
		if (taktclick > 0) taktclick--;

		// �������� ������� �� �����, ��� �������� ������� ������ �������


		if (lb && mousePos.X >= 127 && mousePos.X <= 134
			&& mousePos.Y >= 52 && mousePos.Y <= 54)
		{
			Enemy::kills = 0;
			Tower::money = 200;
			Tower::lost = 0;
			Enemy::countEnemy = 0;
			Tower::countTower = 0;
			end = 0;
			win = 0;
			lb = 0;
			EV.clear();
			TV.clear();
			Unit::takt = 1;

			/*	Enemy E1(Enemy1, 4, 1, 10, MoveRight);
			Enemy E2(Enemy2, 4, 1, 10, MoveRight);
			Enemy E3(Enemy3, 4, 1, 10, MoveRight); */

		}


		if (lb && Tower::select && !Tower::tobuy)
		{
			for (int i = 0; i < Tower::countTower; i++)
			{
				if (!Tower::tobuy && mousePos.X >= TV[i].GetX() && mousePos.X < TV[i].GetX() + towerWidth
					&& mousePos.Y >= TV[i].GetY() && mousePos.Y < TV[i].GetY() + towerHigh)  // ��� ����� � �������� �� ����� ����
				{
					TV[i].BuyBullets(10);
					lb = 0;
				}
			}

		}

		// �������� ������� �� ������� ����� �� ������� ������
		if (lb && Tower::select && mousePos.X >= cols - textShift && mousePos.X < cols - textShift + towerWidth
			&& mousePos.Y >= panelShift && mousePos.Y < Turet.cost  // ��� ���� ����� ���������� ����� ������ ������
			)  // � �������� ������ ����� ������ ��������� �����
		{
			////ShellExecuteA(NULL, "open", "wavplay.exe", "buytower.wav", NULL, 0);
			Tower::tobuy = true; //����� ��������� ��������� �����
			Tower::select = false; //��������� ������� ����� ������
			Tower::numbuy = 1; // ����� ����� ����� ����������
			turetPanel.blood = true;
			turetPanel.DrawPanel();

			teslaPanel.blood = false;
			teslaPanel.DrawPanel();
			lb = 0;
		}

		if (lb && Tower::select && mousePos.X >= cols - textShift + space + towerWidth && mousePos.X < cols - textShift + 2 * (towerWidth + space)
			&& mousePos.Y >= panelShift && mousePos.Y < Tesla.cost  // ��� ���� ����� ���������� ����� ������ ������
			)  // � �������� ������ ����� ������ ��������� �����
		{
			////ShellExecuteA(NULL, "open", "wavplay.exe", "buytower.wav", NULL, 0);
			Tower::tobuy = true; //����� ��������� ��������� �����
			Tower::select = false; //��������� ������� ����� ������
			Tower::numbuy = 2; // ����� ����� ����� ����������

			teslaPanel.blood = true;
			teslaPanel.DrawPanel();

			turetPanel.blood = false;
			turetPanel.DrawPanel();


			lb = 0;
		}

		else if (lb && Tower::tobuy)
		{
			switch (Tower::numbuy)
			{
			case 1:
				if (Tower::money < Turet.cost)
				{
					////ShellExecuteA(NULL, "open", "wavplay.exe", "sade.wav", NULL, 0);
				}
				else if (mousePos.X >= space && mousePos.X < cols - textShift - towerWidth - space
					&& mousePos.Y >= space && mousePos.Y < lines - towerHigh)
				{
					TV.push_back(Turet); // ��� ������� ������ ����� �� ������� ������
					TV[Tower::countTower].Reset(mousePos.X, mousePos.Y);
					Tower::money -= TV[Tower::countTower].cost;
					Tower::countTower++; //����������� �����, ������� ���������� ����� ���� �� ������� �������, �������� ������ �������, � ����� ����� ��������� � �����
					////ShellExecuteA(NULL, "open", "wavplay.exe", "build.wav", NULL, 0);

				}

				turetPanel.blood = false;
				turetPanel.DrawPanel();
				break;

			case 2:
				if (Tower::money < Tesla.cost)
				{
					////ShellExecuteA(NULL, "open", "wavplay.exe", "sade.wav", NULL, 0);
				}
				else if (mousePos.X >= space && mousePos.X < cols - textShift - towerWidth - space
					&& mousePos.Y >= space && mousePos.Y < lines - towerHigh)
				{
					TV.push_back(Tesla); // ��� ������� ������ ����� �� ������� ������
					TV[Tower::countTower].Reset(mousePos.X, mousePos.Y);
					Tower::money -= TV[Tower::countTower].cost;
					Tower::countTower++; //����������� �����, ������� ���������� ����� ���� �� ������� �������, �������� ������ �������, � ����� ����� ��������� � �����
					////ShellExecuteA(NULL, "open", "wavplay.exe", "build.wav", NULL, 0);
				}
				teslaPanel.blood = false;
				teslaPanel.DrawPanel();

			}
			Tower::tobuy = false;
			Tower::select = true; //��������� ������� ����� ������
			lb = 0;
			continue;
		}


		//���������� ������ ��� ������ ����� � ������ ����� ���������� ������
		//��� �������� ������ ������ ��� ����� ������� ������� ������ ���������� ������,
		//�� ���� ������ ������� ������� �� ��� ����� ������� � ��������, �������� � ������� ���������
		//��� ��� ��� ���� ������������ ���������������� ������� �������
		//Scr.ChangeStr("Kills:    ",cols - 25, 0 , 13);  //������ � � ������� ������� �����, ����� �� ��������� ����� ���� ����� �������� �� 100 ����������� �� 99
		//Scr.ChangeInt(Enemy :: kills, cols - 20, 0 , 13); 		
		//Scr.ChangeStr("Money:    ", cols - 25, 1 , 13); 
		//Scr.ChangeInt(Tower :: money, cols - 20, 1 , 13); 		
		//Scr.ChangeStr("Lost:    ", 61, 2 , 13); 
		//Scr.ChangeInt(Tower :: lost, 70, 2 , 13); 		

		cursorPos.X = cols - textShift;
		cursorPos.Y = 16;
		SetConsoleCursorPosition(H, cursorPos);
		SetConsoleTextAttribute(H, 7);
		CharToOemA("������:", str);
		printf("%s", str);

		cursorPos.X = cols - textShift;
		cursorPos.Y = 17;
		SetConsoleCursorPosition(H, cursorPos);
		SetConsoleTextAttribute(H, 7);
		CharToOemA("�����.:", str);
		printf("%s%2d", str, Turet.range);

		cursorPos.X = cols - textShift;
		cursorPos.Y = 18;
		SetConsoleCursorPosition(H, cursorPos);
		SetConsoleTextAttribute(H, 7);
		CharToOemA("����:", str);
		printf("%s%2d", str, Turet.pow);

		cursorPos.X = cols - textShift;
		cursorPos.Y = 19;
		SetConsoleCursorPosition(H, cursorPos);
		SetConsoleTextAttribute(H, 7);
		CharToOemA("�����:", str);
		printf("%s%2d", str, Turet.countTarget);

		cursorPos.X = cols - textShift;
		cursorPos.Y = 20;
		SetConsoleCursorPosition(H, cursorPos);
		SetConsoleTextAttribute(H, 7);
		CharToOemA("�������:", str);
		printf("%s%2d", str, Turet.bullets);

		cursorPos.X = cols - textShift;
		cursorPos.Y = 21;
		SetConsoleCursorPosition(H, cursorPos);
		SetConsoleTextAttribute(H, 7);
		CharToOemA("����:", str);
		printf("%s%2d", str, Turet.cost);

		//////////////////////////////////
		cursorPos.X = cols - textShift + space + towerWidth;
		cursorPos.Y = 16;
		SetConsoleCursorPosition(H, cursorPos);
		SetConsoleTextAttribute(H, 7);
		CharToOemA("�����:", str);
		printf("%s", str);

		cursorPos.X = cols - textShift + space + towerWidth;
		cursorPos.Y = 17;
		SetConsoleCursorPosition(H, cursorPos);
		SetConsoleTextAttribute(H, 7);
		CharToOemA("�����.:", str);
		printf("%s%2d", str, Tesla.range);

		cursorPos.X = cols - textShift + space + towerWidth;
		cursorPos.Y = 18;
		SetConsoleCursorPosition(H, cursorPos);
		SetConsoleTextAttribute(H, 7);
		CharToOemA("����:", str);
		printf("%s%2d", str, Tesla.pow);

		cursorPos.X = cols - textShift + space + towerWidth;
		cursorPos.Y = 19;
		SetConsoleCursorPosition(H, cursorPos);
		SetConsoleTextAttribute(H, 7);
		CharToOemA("�����:", str);
		printf("%s%2d", str, Tesla.countTarget);

		cursorPos.X = cols - textShift + space + towerWidth;
		cursorPos.Y = 20;
		SetConsoleCursorPosition(H, cursorPos);
		SetConsoleTextAttribute(H, 7);
		CharToOemA("�������:", str);
		printf("%s%2d", str, Tesla.bullets);

		cursorPos.X = cols - textShift + space + towerWidth;
		cursorPos.Y = 21;
		SetConsoleCursorPosition(H, cursorPos);
		SetConsoleTextAttribute(H, 7);
		CharToOemA("����:", str);
		printf("%s%2d", str, Tesla.cost);






		//////////////////////////////////////

		cursorPos.X = cols - textShift;
		cursorPos.Y = 52;
		SetConsoleCursorPosition(H, cursorPos);
		SetConsoleTextAttribute(H, 7);
		CharToOemA("--------", str);
		printf("%s", str);

		cursorPos.X = cols - textShift;
		cursorPos.Y = 53;
		SetConsoleCursorPosition(H, cursorPos);
		SetConsoleTextAttribute(H, 7);
		CharToOemA("|������|", str);
		printf("%s", str);

		cursorPos.X = cols - textShift;
		cursorPos.Y = 54;
		SetConsoleCursorPosition(H, cursorPos);
		SetConsoleTextAttribute(H, 7);
		CharToOemA("--------", str);
		printf("%s", str);

		cursorPos.X = cols - textShift;
		cursorPos.Y = 0;
		SetConsoleCursorPosition(H, cursorPos);
		SetConsoleTextAttribute(H, 7);
		CharToOemA("������ �����: ", str);
		printf("%s%3d", str, Enemy::kills);

		cursorPos.X = cols - textShift;
		cursorPos.Y = 1;
		SetConsoleCursorPosition(H, cursorPos);
		SetConsoleTextAttribute(H, 7);
		CharToOemA("��������� �����: ", str);
		printf("%s%3d", str, Tower::money);

		cursorPos.X = cols - textShift;
		cursorPos.Y = 2;
		SetConsoleCursorPosition(H, cursorPos);
		SetConsoleTextAttribute(H, 7);
		CharToOemA("����� ���������: ", str);
		printf("%s%3d", str, Tower::lost);

		cursorPos.X = cols - textShift;
		cursorPos.Y = 4;
		SetConsoleCursorPosition(H, cursorPos);
		SetConsoleTextAttribute(H, 7);
		CharToOemA("����� ���������: ", str);
		printf("%s%2d", str, Tower::countTower);

		cursorPos.X = cols - textShift;
		cursorPos.Y = 5;
		SetConsoleCursorPosition(H, cursorPos);
		SetConsoleTextAttribute(H, 7);
		CharToOemA("��������� ������ : ", str);
		printf("%s%2d", str, Enemy::countEnemy);

		//////////////////////////





		if (end)
		{
			if (win)
			{
				cursorPos.X = cols - textShift;
				cursorPos.Y = 50;
				SetConsoleCursorPosition(H, cursorPos);
				SetConsoleTextAttribute(H, 7);
				CharToOemA("������. ������� 1000!!!", str);
				printf("%s", str);
			}

			else
			{
				cursorPos.X = cols - textShift;
				cursorPos.Y = 50;
				SetConsoleCursorPosition(H, cursorPos);
				SetConsoleTextAttribute(H, 7);
				CharToOemA("    ����� ����!!!     ", str);
				printf("%s", str);
			}



			Enemy::countEnemy = 0;
			Tower::countTower = 0;

		}

		else
		{

			cursorPos.X = cols - textShift;
			cursorPos.Y = 50;
			SetConsoleCursorPosition(H, cursorPos);
			SetConsoleTextAttribute(H, 7);
			CharToOemA("�������� 1000 �����!", str);
			printf("%s", str);
		}

		if (_kbhit())
		{
			code = getch();
			/*
			if (code  == 224)
			{
			switch (getch())
			{
			case LEFT:
			E1.direction = MoveLeft;
			break;
			case RIGHT:
			E1.direction = MoveRight;
			break;
			case UP:
			E1.direction = MoveUp;
			break;
			case DOWN:
			E1.direction = MoveDown;
			break;
			}
			}
			else if (code == 32)
			{
			E1.Target (1);
			}
			else if (code == 13)
			{
			T1.Fire();
			}
			else if (code == '1')
			{
			T1.Target (1);
			}

			else if (code == '2')
			{
			E1.Fire();
			}

			else if (code == '3')
			{
			T1.SetState (6) ;
			}

			else if (code == '4')
			{
			T1.SetState (8) ;
			}

			else if (code == '8')
			{
			break ;
			}*/

			if (code == '32')
			{


			}
		}

		R1.Draw(0); //��������� ����� �������  ������ ��� ���� �����
		//������� ������� ������ ���� ����� ����� ����� �� ����� ������ ����
		//���� ����� �������� ���� �������� ���� ������, ������� ����� ������ ���� ���������
		//���� ����� ����� - ������ ���������




		// ���������� ������ ���� ����� � ������� ��� ��������
		if (!end)
		{
			for (int i = 0; i < Tower::countTower; i++)
			{
				TV[i].ReDraw();
				if (TV[i].deleted) //��� ������� �����
				{
					Tower::lost++;
					TV.erase(TV.begin() + i);
					Tower::countTower--;
					break;
				}
				if (TV[i].bullets > 0 && TV[i].charge != 3)
					TV[i].Recharge();
			}

			//���� �������� ����� �� ���� � ����� �����
			//�������� ���� ������ � ������ - ���� � �����

			for (int i = 0; i < Tower::countTower; i++)
			{
				tj = 0;
				if (TV[i].findtarget) tj = TV[i].numtarget;
				for (; tj < Enemy::countEnemy; tj++)
				{
					if (abs(EV[tj].GetX() - TV[i].GetX()) < TV[i].range
						&& abs(EV[tj].GetY() - TV[i].GetY()) < TV[i].range
						&& EV[tj].state > 3
						&& TV[i].state > 3
						)
					{
						if (EV[tj].GetX() + 5 < TV[i].GetX()
							&& EV[tj].GetY() + 5 < TV[i].GetY())
							TV[i].SetState(4);
						else if (EV[tj].GetX() + 5 >= TV[i].GetX() && EV[tj].GetX() - 5 < TV[i].GetX()
							&& EV[tj].GetY() < TV[i].GetY())
							TV[i].SetState(6);
						else if (EV[tj].GetX() - 5 > TV[i].GetX()
							&& EV[tj].GetY() + 5 < TV[i].GetY())
							TV[i].SetState(8);
						else if (EV[tj].GetX() > TV[i].GetX()
							&& EV[tj].GetY() + 5 >= TV[i].GetY() && EV[tj].GetY() - 5 < TV[i].GetY()
							)
							TV[i].SetState(10);
						else if (EV[tj].GetX() - 5 > TV[i].GetX()
							&& EV[tj].GetY() > TV[i].GetY()
							)
							TV[i].SetState(12);
						else if (EV[tj].GetX() + 5 >= TV[i].GetX() && EV[tj].GetX() - 5 < TV[i].GetX()
							&& EV[tj].GetY() > TV[i].GetY())
							TV[i].SetState(14);
						else if (EV[tj].GetX() + 5 < TV[i].GetX()
							&& EV[tj].GetY() - 5 > TV[i].GetY())
							TV[i].SetState(16);
						else if (EV[tj].GetX() < TV[i].GetX()
							&& EV[tj].GetY() + 5 >= TV[i].GetY() && EV[tj].GetY() - 5 < TV[i].GetY()
							)
							TV[i].SetState(18);
						if (TV[i].findtarget == true) break;
						TV[i].numtarget = tj; //���������� ����
						TV[i].findtarget = true;
						TV[i].timeout = 10; //�������� ������
						if (TV[i].findtarget == true) break;
					}
					else {
						TV[i].findtarget = false;
						TV[i].timeout = 0; //
					}
				}

				if (TV[i].numtarget >= Enemy::countEnemy) TV[i].findtarget = false;
				if (!TV[i].timeout && TV[i].findtarget && TV[i].charge == 3)
				{
					TV[i].Fire(); //��������
					EV[TV[i].numtarget].Target(TV[i].pow);
					TV[i].timeout = 0;
				}
				if (TV[i].timeout)
					TV[i].timeout--;
			}

			// ���� ���������� ���� ������, ����������� �� ���� ������ � ������� ������ ������� ��� ��������
			for (int j = 0; j < Enemy::countEnemy && !end; j++)
			{
				EV[j].Move();
				EV[j].Recharge();
				if (EV[j].moves == 18) EV[j].direction = MoveDown;
				if (EV[j].moves == 36) EV[j].direction = MoveRight;
				if (EV[j].moves == 55) EV[j].direction = MoveUp;
				if (EV[j].moves == 73) EV[j].direction = MoveRight;
				if (EV[j].moves == 150) EV[j].direction = MoveDown;
				if (EV[j].moves == 163) EV[j].direction = MoveLeft;
				if (EV[j].moves == 222) EV[j].direction = MoveDown;
				if (EV[j].moves == 240) EV[j].direction = MoveLeft;
				if (EV[j].moves == 294) EV[j].direction = MoveDown;
				if (EV[j].moves == 307) EV[j].direction = MoveRight;
				if (EV[j].moves == 379) EV[j].direction = MoveUp;
				if (EV[j].moves == 397) EV[j].direction = MoveRight;


				if (EV[j].GetX() == 115 || EV[j].GetY() == 50)
					end = true;

				if (EV[j].deleted)
				{
					////ShellExecuteA(NULL, "open", "wavplay.exe", "money.wav", NULL, 0);
					//EV[j].Reset(5, 5, MoveRight);

					Tower::money += EV[j].killmoney;
					Enemy::kills++;

					EV.erase(EV.begin() + j);
					Enemy::countEnemy--;
					//Enemy::countEnemy --;
					//EV.erase(EV.begin() + j);
					//EV.insert(EV.begin() + j,E2);
				}

			}
			if (Unit::takt % 50 == 0)
			{
				EV.push_back(E1);
				Enemy::countEnemy++;
			}

			if ((Tower::money > 200 || Tower::countTower > 3 || Enemy::kills > 15) && Unit::takt % 200 == 0)
			{
				EV.push_back(E2);
				Enemy::countEnemy++;
			}

			if ((Tower::money > 300 || Tower::countTower > 6) && Unit::takt % 400 == 0)
			{
				EV.push_back(E3);
				Enemy::countEnemy++;
			}

			if ((Tower::money > 400 || Tower::countTower > 12) && Unit::takt % 35 == 0)
			{
				EV.push_back(E1);
				Enemy::countEnemy++;
			}

			if ((Tower::money > 600 || Tower::countTower > 16) && Unit::takt % 700 == 0)
			{
				EV.push_back(E3);
				Enemy::countEnemy++;
			}

			/*if (Unit::takt == 10)
			{
			EV.push_back(E1);
			Enemy::countEnemy ++;
			}*/

			//���� �������� ��������� ����� � ����� ������
			//��������  ������ � ������ - ���� � �����
			for (int j = 0; j < Enemy::countEnemy; j++)
			{
				ei = 0;
				if (EV[j].findtarget) ei = EV[j].numtarget;
				for (; ei < Tower::countTower; ei++)
				{
					if (abs(EV[j].GetX() - TV[ei].GetX()) < EV[j].range
						&& abs(EV[j].GetY() - TV[ei].GetY()) < EV[j].range
						&& EV[j].charge == 3
						&& EV[j].state > 3
						&& TV[ei].state > 3
						)
					{
						if (EV[j].findtarget == true) break;
						EV[j].numtarget = ei; //���������� ����
						EV[j].findtarget = true;
						EV[j].timeout = 10; //�������� ������
						if (EV[j].findtarget == true) break;
					}
					else {
						EV[j].findtarget = false;
						EV[j].timeout = 0;
					}
				}
				if (EV[j].numtarget >= Tower::countTower) EV[j].findtarget = false;
				if (!EV[j].timeout && EV[j].findtarget && EV[j].charge == 3)
				{
					EV[j].Fire(); //��������
					TV[EV[j].numtarget].Target(EV[j].pow);
					EV[j].timeout = 0;
				}
				if (EV[j].timeout)
					EV[j].timeout--;
			}
		}
		lb = false; //����� ������ ������� ����
		//rb= false;
		Sleep(50);
		//system ("cls");
	}
	SetConsoleTextAttribute(H, csbi.wAttributes); //���������� ��������� ����� �������
}