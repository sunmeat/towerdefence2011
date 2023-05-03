#pragma once
#include "Object.h"
class Unit : public Object

{
	int taktFire; 
	int taktTarget; 
	int taktKilling;
	int taktCharging;

	
	bool target; //флаг под огнем
	int armor ; //при попадании в данный объект число отнимаемое от countTarget
	char * soundFire;
	char * soundTarget;
	char * soundDeth;

	void RestoreCountTarget()
	{
		char digit [3] = {str[11], str[12], '\0'};
		countTarget = atoi (digit);
	}

	void State (int _state) 
	{
		if (taktFire && !(state % 2)) 
		{
			taktFire --;
			state ++; 
		}
		else if (taktFire && state % 2) 
		{
			taktFire --;
			state --; 
		}

		else if (!taktFire &&state >3 && state % 2) 
			state --; 

		else if (taktTarget && !blood)
		{
			taktTarget --;
			blood = true;
		}
		else if (taktTarget && blood)
		{
			taktTarget --;
			blood = false;
		}
		else if (!taktTarget && blood)
			blood = false;

		else if (taktKilling)
			taktKilling --;

		else if (taktDeth)
			taktDeth --;

		if (state != _state) 
			changeState = true;
		else 
			changeState = false;
	}

protected: 
	int taktDeth; 
	int damage; // разрушения пушки (три уровня: 0- не окрашено ничего , 1 , 2 , 3)

	bool fire; // стреляющий в текущий момент

public:
	bool blood;
	int countTarget; //количество раз попаданий для одного уменьшения брони - типа броня

	int damageline; // 2 цифры номера колонки где будет подстветка разрушения
	int rechargeline; // 2 цифры номера колонки где будет подстветка заряда
	int pow; // уровень атаки
	bool findtarget; //флаг что цель найдена
	int range ; // дальность огня
	static int takt ; //отчет тактов игрового процесса
	int charge; // заряд пушки (три уровня: 0- не окрашено ничего , 1 , 2 , 3)
	int numtarget; // номер захваченной цели, стреляет в нее пока не потеряет из виду
	int timeout; // число тактов для задержки перед действием

	void Target(int i) 
	{
		target = true; //флаг что под огнем
		armor = i; // сила удара от врага , у врага она равна pow
	}

	int Fire() 
	{
		fire = true; //флаг что стреляет
		return pow;
	} 

	void Draw () 
	{
		if (deleted) 
			return;
		if (fire && state >3 && charge ==3) //юнит стреляет
		{
			//ShellExecuteA(NULL,"open","wavplay.exe", this->soundFire, NULL, 0) ;
			//sndPlaySound( this->soundFire, SND_ASYNC );
			taktFire = 8;
			charge = 0;
		}

		if (target && state >3 && countTarget > 0 /*&& damage >=0*/ ) //объект под огнем
		{
			countTarget -= armor;
			taktTarget = 11;
		}

		if (target && state >3 && countTarget < 1 && damage > 0)
		{ 
			// я передумал вывводить звук "ПОД ОГНЕМ" только мешает и так видно красным
			////ShellExecuteA(NULL,"open","wavplay.exe", this->soundTarget, NULL, 0) ;
			RestoreCountTarget();
			damage --;
			taktTarget = 5;
		}

		//if (target && state > 3 && countTarget > 0 && damage < 0 )
		//{
		//	countTarget -= armor ;
		//	taktTarget = 5;
		//}

		if (target && state > 3 && countTarget < 1 && damage == 0 )
		{
			//ShellExecuteA(NULL,"open","wavplay.exe", this->soundDeth, NULL, 0) ;
			//sndPlaySound( this->soundDeth , SND_ASYNC);
			taktKilling = 3;
			state = 1;
			charge = 0;
		}

		if (!taktKilling && state == 1 )
		{
			taktKilling = 3;
			state = 2;
		}

		if (!taktKilling && state == 2 )
		{
			taktKilling = 3;
			state = 3;
		}

		if (!taktKilling && state == 3 )
		{
			taktDeth = 20;
			state = 0;
		}

		if (!taktDeth && state == 0) 
		{
			deleted = 1;
		}

		target = false;
		fire = false;

		State(state); //проверка и выбор нужного слайда

		if (changeState  || m == nullptr)
		{
			for (int i = index[state]; str [i] != '~' ; i++) 
				if ( str[i] !='\n')
					size ++; //количество точек
			if (m) delete [] m;
			m = new Point [size] ;
		}
		int x = 0; 
		int y = 0;
		int count = 0; // текущая точка
		for (int i = index[state]; str [i] != '~' ; i++) {

			if(str[i] =='\n') 
			{
				y++;
				x = 0;
				continue;
			}

			m[count].Set(X + x,Y + y, str[i], blood ? 12 : color) ;

			if (charge > 0 && y == 4 && x == rechargeline) 	m[count].SetColor(31);
			if (charge > 1 && y == 3 && x == rechargeline) 	m[count].SetColor(31);
			if (charge > 2 && y == 2 && x == rechargeline) 	m[count].SetColor(31); // подсветка заряда

			if (damage > 0 && y == 4 && x == damageline) 	m[count].SetColor(47);
			if (damage > 1 && y == 3 && x == damageline) 	m[count].SetColor(47);
			if (damage > 2 && y == 2 && x == damageline) 	m[count].SetColor(47); // подсветка разрушения

			if (str[i] == ext) 
				m[count].SetColor(colorExt); // доп. значок 

			count++;
			x++;
		}

		for (int i = 0; i < size; i++) 
			m[i].Show();
	}

	void DrawPanel()
	{
		size = 0;
		for (int i = index[state]; str [i] != '~' ; i++) 
			if ( str[i] !='\n')
				size ++; //количество точек
		//if (m) delete [] m;
		m = new Point [size] ;
		int x = 0; 
		int y = 0;
		int count = 0; // текущая точка
		for (int i = index[state]; str [i] != '~' ; i++) {

			if(str[i] =='\n') 
			{
				y++;
				x = 0;
				continue;
			}

			m[count].Set(X + x,Y + y, str[i], blood ? 12 : color) ;
			count++;
			x++;
		}

		for (int i = 0; i < size; i++) 
			m[i].ShowPanel();
	}

	void Recharge () 
	{
		if (!taktCharging) taktCharging = 10;
		taktCharging --;
		if (!taktCharging && charge <3 && state > 3) charge ++;
	}

	void SetDamage (int i) 
	{
		damage = i;
	}

	int  GetDamage () 
	{
		return damage ;
	}

	void SetCharge(int i) 
	{
		charge = i;
	}

	Unit() : Object () // передаем состояние, координаты и данные о прорисовке юнита
	{
		timeout = 0;
		taktFire = 0;
		taktTarget = 0;
		taktKilling = 0;
		taktCharging = 0;

		taktDeth = 0;
		numtarget =0;
		findtarget = false;
		fire = false;
		target = false;
		blood = false;
		damage = 3;
		charge = 0;

		int line = atoi (str);
		damageline = line / 10;
		rechargeline = line % 10;

		countTarget = atoi (str + 11); //число попаданий для уменьшения брони на 1

		pow = atoi (str + 14);

		range = atoi (str + 23);

		int len;
		int st = 26;
		for (int i = 0; str [i + st] != '\n'; i++)
			len = i + 2 ;

		str [st + len - 1 ] = '\0';
		soundFire = new char [len];
		strcpy (soundFire, str + st);
		st += len;

		for (int i = 0; str [i + st] != '\n'; i++)
			len = i + 2 ;
		str [len + st -1] = '\0';
		soundTarget = new char [len];
		strcpy (soundTarget, str + st );
		st += len;

		for (int i = 0; str [i + st] != '\n'; i++)
			len = i + 2 ;
		str [len + st -1] = '\0';
		soundDeth = new char [len];
		strcpy (soundDeth, str + st);
	}

	Unit(Data unit,  int state, int X, int Y) : Object (unit, state, X, Y) // передаем состояние, координаты и данные о прорисовке юнита
	{
		timeout = 0;
		taktFire = 0;
		taktTarget = 0;
		taktKilling = 0;
		taktCharging = 0;

		taktDeth = 0;
		numtarget =0;
		findtarget = false;
		fire = false;
		target = false;
		blood = false;
		damage = 3;
		charge = 3;

		int line = atoi (str);
		damageline = line / 10;
		rechargeline = line % 10;

		countTarget = atoi (str + 11); //число попаданий для уменьшения брони на 1

		pow = atoi (str + 14);

		range = atoi (str + 23);

		int len;
		int st = 26;
		for (int i = 0; str [i + st] != '\n'; i++)
			len = i + 2 ;

		str [st + len - 1 ] = '\0';
		soundFire = new char [len];
		strcpy (soundFire, str + st);
		st += len;

		for (int i = 0; str [i + st] != '\n'; i++)
			len = i + 2 ;
		str [len + st -1] = '\0';
		soundTarget = new char [len];
		strcpy (soundTarget, str + st );
		st += len;

		for (int i = 0; str [i + st] != '\n'; i++)
			len = i + 2 ;
		str [len + st -1] = '\0';
		soundDeth = new char [len];
		strcpy (soundDeth, str + st);
	}

	Unit ( const Unit & ob) : Object (ob)
	{
		timeout = ob.timeout;
		taktFire = ob.taktFire;
		taktTarget = ob.taktTarget;
		taktKilling = ob.taktKilling;
		taktCharging = ob.taktCharging;

		taktDeth = ob.taktDeth;
		numtarget =ob.numtarget;
		findtarget = ob.findtarget;
		fire = ob.fire;
		target = ob.target;
		blood = ob.blood;
		damage = ob.damage;
		charge = ob.charge;
		damageline = ob.damageline;
		rechargeline = ob.rechargeline;

		countTarget = ob.countTarget; //число попаданий для уменьшения брони на 1

		pow = ob.pow;

		range = ob.range;

		soundFire = new char [strlen(ob.soundFire) + 1];
		strcpy (soundFire, ob.soundFire);

		soundTarget = new char [strlen(ob.soundTarget) + 1];
		strcpy (soundTarget, ob.soundTarget);

		soundDeth = new char [strlen(ob.soundDeth) + 1];
		strcpy (soundDeth, ob.soundDeth);

	}

	~Unit(void)
	{
	}
};

int Unit::takt ;