/*
* ygcGo19.cpp
* default rules implementation
*/

#include "pch.h"

using namespace ygc;

Go19::Go19Match::Go19Match()
{
	board = ref new ygcBoard(Go19::Go19BoardWidth, Go19::Go19BoardHeight);
	board->moveHistory = moveHistory;

	for (auto i = 0; Go19::defaultMoveValidators[i]; ++i)
		matchRules->validMoves.push_back(Go19::defaultMoveValidators[i]);

	for (auto i = 0; Go19::defaultPostMoveActions[i]; ++i)
		matchRules->postMoves.push_back(Go19::defaultPostMoveActions[i]);

	turn = ref new Go19::Go19StoneColor(Go19::Go19StoneColor::BLACK);
}

bool Go19::bGo19_IsEmpty(ygcBoard^ b, uint16_t x, uint16_t y)
{
	if (*b->getAt(x,y)->takenBy != Go19StoneColor::EMPTY)
		return false;
	return true;
}

bool Go19::bGo19_HasLiberties(ygcBoard^, uint16_t, uint16_t)
{
	return true;
}

void Go19::vGo19_SearchForPrisoners(ygcMatch^, uint16_t, uint16_t)
{

}