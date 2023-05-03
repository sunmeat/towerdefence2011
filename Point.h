#pragma once

#include <windows.h>
#include <iostream>
#include "Screen.h"

Screen Scr (cols, lines);
HANDLE M = GetStdHandle(-10);

class Point
{
	int x,y;
	int color;
	char symbol;
	int layer;

public:
	Point()
	{
		cursorPos.X = 0; 
		cursorPos.Y = 0;
		color = 15;
		symbol = '*';
		layer = 0;
	}

	Point (int _x, int _y, char _symbol, int _color) 
	{
		x = _x;
		y = _y;
		symbol = _symbol;
		color = _color;
	}
	
	Point (int _x, int _y) 
	{
		cursorPos.X = _x; 
		cursorPos.Y = _y;
		color = 15;
		symbol = '*';
	}

	void Show  () 
	{
		Pix.color = color;
		Pix.x = x;
		Pix.y = y;
		Pix.layer = layer;
		Pix.symbol = symbol;
		Scr.ChangePixel(Pix);
	}

	void ShowPanel  () 
	{
		cursorPos.X = x; 
		cursorPos.Y = y;
		SetConsoleCursorPosition(H, cursorPos);
		SetConsoleTextAttribute(H, color);
		if (symbol == '_') symbol = ' ';
		putchar (symbol);
	}

	Point & operator = (const Point & ob) 
	{
		x = ob.x;
		y = ob.y;
		symbol = ob.symbol;
		color = ob.color;
		return * this;
	}

	Point (const Point & ob) 
	{
		x = ob.x;
		y = ob.y;
		symbol = ob.symbol;
		color = ob.color;
	}

	void Set (int _x, int _y, char _symbol, int _color) 
	{
		x = _x;
		y = _y;
		symbol = _symbol;
		color = _color;
	}

	void SetColor (int _color) 	{color = _color;}
	int GetY () {return y;}
	int GetX () {return x;}
	void Right (int i) {x += i;}
	void Left (int i) {	x -= i;}
	void Up (int i) {y -= i;}
	void Down (int i) {	y += i;}

	~Point(void)
	{
	}
};

void Change(Screen Scr, Pixel Pix) {Scr.ChangePixel(Pix);}