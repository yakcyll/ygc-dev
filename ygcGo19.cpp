/*
* ygcGo19.cpp
* default rules implementation
*/

#include "pch.h"

using namespace ygc;

bool Go19::bGo19_IsEmpty(ygcBoard^ b, uint16_t x, uint16_t y)
{
	if (b->fields[x][y]->takenBy != Go19StoneColor::EMPTY)
		return false;
	return true;
}

bool Go19::bGo19_HasLiberties(ygcBoard^, uint16_t, uint16_t)
{
	return true;
}

void Go19::vGo19_SearchForPrisoners(ygcBoard^, uint16_t, uint16_t)
{

}