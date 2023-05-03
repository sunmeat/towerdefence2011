# define LEFT 75  // для управления курсором для менюшки
# define RIGHT 77
# define UP 72
# define DOWN 80

#define pathTuret "turet.txt" //для удобства путей
#define pathTesla "tesla.txt" //для удобства путей

#define pathEnemy "enemy.txt"
#define pathEnemy2 "enemy2.txt"
#define pathEnemy3 "enemy3.txt"

#define pathRoad "road.txt"

const int lines = 60; // размеры окна
const int cols = 150;

struct Data  //структура данных для прорисовки объекта и его характеристиках
{
	char* strData; //указатель на данные, загруженные в память из файлов по всем состояниям(слайдам) объекта
	int* index; //индексы начала данных слайда для каждого состояния юнита
	int count; // количество слайдов объекта
	int size; // размер всего массива данных со слайдами
};

struct Pixel //структура с данными о клетке ("пикселе") псевдографического экрана
{
	char symbol;
	int x, y;
	int layer;
	int color;
	bool changed;
};

#include "Object.h" // класс любых объектов будь-то дорога, башня или враг или целая готовая карта поля сражений
#include "Unit.h"	// класс юнитов будь то башня или движущийся враг
#include "Enemy.h"  // класс врагов 
#include "Tower.h" //класс башен
#include "Screen.h" ///класс объекта-где хранится двумерный массив символов псевдографики текущего отображения всего поля  сражения методов прорисовки псевдографики

#include <conio.h>
#include <vector>

using namespace std;

// функция для загрузки данных со слайдами и харектеристиками объектов из файлов
// принимает путь на файл из которого грузить и по ссылке в какую переменную структуры Data записать полученные данные из файла
void LoadPics(char* str, Data& object)
{
	FILE* f = fopen(str, "r");
	if (!f) return; // если файла нет - печаль
	fseek(f, 0, SEEK_END); // перемещаем курсор в конец файла
	object.size = ftell(f); // длина данных с рисунками
	object.strData = new char[object.size]; // выделяем строку под это количество символов
	rewind(f); // переводим курсор в начало файла
	fread(object.strData, sizeof(char), object.size, f);
	object.count = 0;
	for (int i = 0; i < object.size; i++) // 
		if (object.strData[i] == '$') object.count++;  //подсчет сколько слайдов
	object.index = new int[object.count]; // создали массив для хранения индексов стартов слайдов
	for (int k = 0, i = 0; i < object.size; i++) // 
		if (object.strData[i] == '$')
		{
			object.index[k] = i + 2; //заполнили индексы с началом картинок
			k++; // i+2 второго потому что i-ый это сам $ а i+1 это символ \r
		}
}

void color() //просто функция для того чтобы посмотреть подходящий цвет
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

//мультипроцесс обработчик нажатий мышки
//постоянно опрашивает движение мышки и результат записывает в принятый указатель event
DWORD WINAPI Mouse(void* p)
{
	DWORD click;
	while (1)
		ReadConsoleInput(M, (INPUT_RECORD*)p, 1, &click);
	return 0;
}

//мультипроцесс постоянная прорисовка поля битвы только тех клеток где был изменен символ
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
	GetConsoleScreenBufferInfo(H, &csbi); //запоминаем текущий цвет консоли
	SetConsoleMode(M, ENABLE_MOUSE_INPUT | ENABLE_EXTENDED_FLAGS);

	CONSOLE_CURSOR_INFO cci;
	cci.bVisible = 0;
	cci.dwSize = 100;

	HWND hwnd = GetConsoleWindow();
	MoveWindow(hwnd, 35, 15, 1280, 800, true);

	SetConsoleCursorInfo(H, &cci);

	INPUT_RECORD event; //тут будут хранится постоянно меняющиеся события от мыши в другом процессе

	Data turet;
	LoadPics(pathTuret, turet); //грузим тип башни номер 1
	Data tesla;
	LoadPics(pathTesla, tesla); //грузим тип башни номер 2


	Data Enemy1;
	LoadPics(pathEnemy, Enemy1); //грузим тип врага номер 1

	Data Enemy2;
	LoadPics(pathEnemy2, Enemy2); //грузим тип врага номер 1

	Data Enemy3;
	LoadPics(pathEnemy3, Enemy3); //грузим тип врага номер 1


	Data Road1;
	LoadPics(pathRoad, Road1); //грузим тип объекта номер 1 - это может быть готовая карта

	Unit::takt = 0; //подсчет тактов цикла возможных изменений состояний любого объекта (юнита)

	const int speedcharge = 10; //общая скорость - отсчет числа тактов для одного изменения шкалы перезарядки для следующего выстрела у любых юнитов

	vector <Tower> TV; //  объявляем веторы - массивы для будуищих юнитов
	vector <Enemy> EV; //  

	Object R1(Road1, 0, 0, 0); //так можно и без вектора создать одну готовую карту битвы

	// следующие переменные будут нулевые либо с определенным стартовым значением при старте игрового процесса
	//статик переменные классов
	Unit::takt = 0;
	Tower::money = 200; //деньги игрока, хранятся в статик переменной
	Tower::countTower = 0; //текущее количество отображающихся башен
	Enemy::countEnemy = 0; // текущее количество отображающихся врагов
	Enemy::kills = 0; // количество смертей любого врага
	Tower::lost = 0; // количество смертей любой башни

	Enemy E1(Enemy1, 4, 0, 2, MoveRight);
	Enemy E2(Enemy2, 4, 0, 2, MoveRight);
	Enemy E3(Enemy3, 4, 0, 2, MoveRight);



	Tower Turet(turet, 4, cols - 15, 22); // так будет создаваться башня определенного типа, в определенном месте 
	Tower Tesla(tesla, 4, cols - 15, 22); // так будет создаваться башня определенного типа, в определенном месте 
	//Tower T2(Tower1, 4, 40, 20); // это будет башня другого типа, 
	//все возможные типы башен и врагов должны быть созданы  начале игры до игрового цикла после выборы карты игры

	//TV.push_back(T1);  //так будут добавляться башни или враги определенного типа на экран и ясно что тогда память будет увеличиваться 

	//TV.push_back(T2);


	/////////////////////////////////////////////////////////////////////////////////

	// запуск других потоков программы
	//CreateThread(0, 0, SreenDraw, &Scr, 0,0); //отображение изменених клеток псевдографики в консоли ( можно и без внешнего потока, но медленнее) 
	CreateThread(0, 0, Mouse, &event, 0, 0);  //обработчик событий мыши

	//вспомогательные переменные для цикла игрового движения

	bool lb = 0; //флаги того какие нажаты кнопки.
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

	int code; //для интерфейса нажатия клавиш пользователем
	int tj; // "tower j " переменная для запоминация порядкового номера башни в случае захвата цели
	int ei; //аналогично "enemy i"
	char str[20];
	COORD mousePos;

	//установка панели выбора башен		
	Tower turetPanel(turet, 4, cols - textShift, panelShift);
	turetPanel.DrawPanel();

	Tower teslaPanel(tesla, 4, cols - textShift + towerWidth + space, panelShift);
	teslaPanel.DrawPanel();

	// собственно сам цикл игрового процесса движухи всех юнитов, отлова нажатий мыши

	while (1)
	{
		if (Unit::takt == 100000)  //нужно для отсчета тактов циклов чтобы отработал какой-то определенный слайд юнита
			Unit::takt = 0;			//короче для задержек не прерывая общий цикл
		else Unit::takt++;

		if (Tower::money >= 1000)
		{
			end = true;
			win = true;
		}

		Scr.Draw(); //прорисовка экрана, то что можно и внешним процессом делать

		mousePos.X = event.Event.MouseEvent.dwMousePosition.X; // чтение координат мышки
		mousePos.Y = event.Event.MouseEvent.dwMousePosition.Y;

		cursorPos.X = cols - textShift;
		cursorPos.Y = 7;
		SetConsoleCursorPosition(H, cursorPos);
		SetConsoleTextAttribute(H, 7);
		CharToOemA("Кординаты: ", str);
		printf("%s%3d, %2d", str, mousePos.X, mousePos.Y);

		if (!taktclick && event.Event.MouseEvent.dwButtonState == FROM_LEFT_1ST_BUTTON_PRESSED)
		{
			lb = true;
			taktclick = 3;
		}
		if (taktclick > 0) taktclick--;

		// проверка нажатия на башни, что повлечет попытку купить патроны


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
					&& mousePos.Y >= TV[i].GetY() && mousePos.Y < TV[i].GetY() + towerHigh)  // тут ловим в пределах ли башен мышь
				{
					TV[i].BuyBullets(10);
					lb = 0;
				}
			}

		}

		// проверка нажатия на область башни из покупок справа
		if (lb && Tower::select && mousePos.X >= cols - textShift && mousePos.X < cols - textShift + towerWidth
			&& mousePos.Y >= panelShift && mousePos.Y < Turet.cost  // тут надо будет определять номер ячейки ячейки
			)  // и согласно номеру брать нужную стоимость башни
		{
			////ShellExecuteA(NULL, "open", "wavplay.exe", "buytower.wav", NULL, 0);
			Tower::tobuy = true; //режим установки купленной башни
			Tower::select = false; //выключили обычный режим выбора
			Tower::numbuy = 1; // какой номер башни покупается
			turetPanel.blood = true;
			turetPanel.DrawPanel();

			teslaPanel.blood = false;
			teslaPanel.DrawPanel();
			lb = 0;
		}

		if (lb && Tower::select && mousePos.X >= cols - textShift + space + towerWidth && mousePos.X < cols - textShift + 2 * (towerWidth + space)
			&& mousePos.Y >= panelShift && mousePos.Y < Tesla.cost  // тут надо будет определять номер ячейки ячейки
			)  // и согласно номеру брать нужную стоимость башни
		{
			////ShellExecuteA(NULL, "open", "wavplay.exe", "buytower.wav", NULL, 0);
			Tower::tobuy = true; //режим установки купленной башни
			Tower::select = false; //выключили обычный режим выбора
			Tower::numbuy = 2; // какой номер башни покупается

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
					TV.push_back(Turet); // тут берется нужная башня по нужному номеру
					TV[Tower::countTower].Reset(mousePos.X, mousePos.Y);
					Tower::money -= TV[Tower::countTower].cost;
					Tower::countTower++; //увеличиваем башни, наверно удаленныую башню буду из вектора удалять, изменять размер вектора, а потом новую добавлять в конец
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
					TV.push_back(Tesla); // тут берется нужная башня по нужному номеру
					TV[Tower::countTower].Reset(mousePos.X, mousePos.Y);
					Tower::money -= TV[Tower::countTower].cost;
					Tower::countTower++; //увеличиваем башни, наверно удаленныую башню буду из вектора удалять, езменять размер вектора, а потом новую добавлять в конец
					////ShellExecuteA(NULL, "open", "wavplay.exe", "build.wav", NULL, 0);
				}
				teslaPanel.blood = false;
				teslaPanel.DrawPanel();

			}
			Tower::tobuy = false;
			Tower::select = true; //выключили обычный режим выбора
			lb = 0;
			continue;
		}


		//используем методы для вывода строк в режиме общей прорисовки экрана
		//это пришлось делать потому что когда внешний процесс гоняет прорисовку экрана,
		//то если менять позицию курсора то она будет прыгать и меняться, путаться с внешним процессом
		//так как там тоже используется позиционирование курсора консоли
		//Scr.ChangeStr("Kills:    ",cols - 25, 0 , 13);  //делаем с с запасом пустого места, чтобы не оставался мусор если число например со 100 уменьшилось до 99
		//Scr.ChangeInt(Enemy :: kills, cols - 20, 0 , 13); 		
		//Scr.ChangeStr("Money:    ", cols - 25, 1 , 13); 
		//Scr.ChangeInt(Tower :: money, cols - 20, 1 , 13); 		
		//Scr.ChangeStr("Lost:    ", 61, 2 , 13); 
		//Scr.ChangeInt(Tower :: lost, 70, 2 , 13); 		

		cursorPos.X = cols - textShift;
		cursorPos.Y = 16;
		SetConsoleCursorPosition(H, cursorPos);
		SetConsoleTextAttribute(H, 7);
		CharToOemA("Турета:", str);
		printf("%s", str);

		cursorPos.X = cols - textShift;
		cursorPos.Y = 17;
		SetConsoleCursorPosition(H, cursorPos);
		SetConsoleTextAttribute(H, 7);
		CharToOemA("Дальн.:", str);
		printf("%s%2d", str, Turet.range);

		cursorPos.X = cols - textShift;
		cursorPos.Y = 18;
		SetConsoleCursorPosition(H, cursorPos);
		SetConsoleTextAttribute(H, 7);
		CharToOemA("Удар:", str);
		printf("%s%2d", str, Turet.pow);

		cursorPos.X = cols - textShift;
		cursorPos.Y = 19;
		SetConsoleCursorPosition(H, cursorPos);
		SetConsoleTextAttribute(H, 7);
		CharToOemA("Броня:", str);
		printf("%s%2d", str, Turet.countTarget);

		cursorPos.X = cols - textShift;
		cursorPos.Y = 20;
		SetConsoleCursorPosition(H, cursorPos);
		SetConsoleTextAttribute(H, 7);
		CharToOemA("Патроны:", str);
		printf("%s%2d", str, Turet.bullets);

		cursorPos.X = cols - textShift;
		cursorPos.Y = 21;
		SetConsoleCursorPosition(H, cursorPos);
		SetConsoleTextAttribute(H, 7);
		CharToOemA("Цена:", str);
		printf("%s%2d", str, Turet.cost);

		//////////////////////////////////
		cursorPos.X = cols - textShift + space + towerWidth;
		cursorPos.Y = 16;
		SetConsoleCursorPosition(H, cursorPos);
		SetConsoleTextAttribute(H, 7);
		CharToOemA("Тесла:", str);
		printf("%s", str);

		cursorPos.X = cols - textShift + space + towerWidth;
		cursorPos.Y = 17;
		SetConsoleCursorPosition(H, cursorPos);
		SetConsoleTextAttribute(H, 7);
		CharToOemA("Дальн.:", str);
		printf("%s%2d", str, Tesla.range);

		cursorPos.X = cols - textShift + space + towerWidth;
		cursorPos.Y = 18;
		SetConsoleCursorPosition(H, cursorPos);
		SetConsoleTextAttribute(H, 7);
		CharToOemA("Удар:", str);
		printf("%s%2d", str, Tesla.pow);

		cursorPos.X = cols - textShift + space + towerWidth;
		cursorPos.Y = 19;
		SetConsoleCursorPosition(H, cursorPos);
		SetConsoleTextAttribute(H, 7);
		CharToOemA("Броня:", str);
		printf("%s%2d", str, Tesla.countTarget);

		cursorPos.X = cols - textShift + space + towerWidth;
		cursorPos.Y = 20;
		SetConsoleCursorPosition(H, cursorPos);
		SetConsoleTextAttribute(H, 7);
		CharToOemA("Патроны:", str);
		printf("%s%2d", str, Tesla.bullets);

		cursorPos.X = cols - textShift + space + towerWidth;
		cursorPos.Y = 21;
		SetConsoleCursorPosition(H, cursorPos);
		SetConsoleTextAttribute(H, 7);
		CharToOemA("Цена:", str);
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
		CharToOemA("|Заново|", str);
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
		CharToOemA("Врагов убито: ", str);
		printf("%s%3d", str, Enemy::kills);

		cursorPos.X = cols - textShift;
		cursorPos.Y = 1;
		SetConsoleCursorPosition(H, cursorPos);
		SetConsoleTextAttribute(H, 7);
		CharToOemA("Накоплено денег: ", str);
		printf("%s%3d", str, Tower::money);

		cursorPos.X = cols - textShift;
		cursorPos.Y = 2;
		SetConsoleCursorPosition(H, cursorPos);
		SetConsoleTextAttribute(H, 7);
		CharToOemA("Башен разрушено: ", str);
		printf("%s%3d", str, Tower::lost);

		cursorPos.X = cols - textShift;
		cursorPos.Y = 4;
		SetConsoleCursorPosition(H, cursorPos);
		SetConsoleTextAttribute(H, 7);
		CharToOemA("Башен построено: ", str);
		printf("%s%2d", str, Tower::countTower);

		cursorPos.X = cols - textShift;
		cursorPos.Y = 5;
		SetConsoleCursorPosition(H, cursorPos);
		SetConsoleTextAttribute(H, 7);
		CharToOemA("Атакующих врагов : ", str);
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
				CharToOemA("Победа. Набрано 1000!!!", str);
				printf("%s", str);
			}

			else
			{
				cursorPos.X = cols - textShift;
				cursorPos.Y = 50;
				SetConsoleCursorPosition(H, cursorPos);
				SetConsoleTextAttribute(H, 7);
				CharToOemA("    Конец игры!!!     ", str);
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
			CharToOemA("Накопить 1000 денег!", str);
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

		R1.Draw(0); //обновляем слайд объекта  дороги или всей карты
		//наверно оставлю только этот слайд общей карты он будет всегда один
		//хотя можно добавить пару объектов типа ВОРОТА, которые могут менять свое состояние
		//типа враги пошли - ворота открылись




		// обновление слайда всех башен и зарядка для стрельбы
		if (!end)
		{
			for (int i = 0; i < Tower::countTower; i++)
			{
				TV[i].ReDraw();
				if (TV[i].deleted) //это покупка башни
				{
					Tower::lost++;
					TV.erase(TV.begin() + i);
					Tower::countTower--;
					break;
				}
				if (TV[i].bullets > 0 && TV[i].charge != 3)
					TV[i].Recharge();
			}

			//цикл проверки попал ли враг в обзор башен
			//проверка идет каждый с каждым - цикл в цикле

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
						TV[i].numtarget = tj; //запоминаем цель
						TV[i].findtarget = true;
						TV[i].timeout = 10; //начинаем отсчет
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
					TV[i].Fire(); //стреляем
					EV[TV[i].numtarget].Target(TV[i].pow);
					TV[i].timeout = 0;
				}
				if (TV[i].timeout)
					TV[i].timeout--;
			}

			// цикл обновления всех врагов, перемещения на одну клетку и зарядка одного деления для стрельбы
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

			//цикл проверки пападания башен в обзор врагов
			//проверка  каждый с каждым - цикл в цикле
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
						EV[j].numtarget = ei; //запоминаем цель
						EV[j].findtarget = true;
						EV[j].timeout = 10; //начинаем отсчет
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
					EV[j].Fire(); //стреляем
					TV[EV[j].numtarget].Target(EV[j].pow);
					EV[j].timeout = 0;
				}
				if (EV[j].timeout)
					EV[j].timeout--;
			}
		}
		lb = false; //сброс флагов нажатий мыши
		//rb= false;
		Sleep(50);
		//system ("cls");
	}
	SetConsoleTextAttribute(H, csbi.wAttributes); //возвращаем начальные цвета консоли
}