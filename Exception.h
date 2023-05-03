#pragma once
#include <string.h>

class Exception
{
public:
	char what[81];

	Exception(char * str)
	{
		strcpy(what,str);
	}

	~Exception(void)
	{
	}
};

