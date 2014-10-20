/*
* ygc.h
* ygc representation implementation
*/

#include "pch.h"

using namespace ygc;


/* ygcMatch method implementation */

ygcMatch::ygcMatch()
{
	board = ref new ygcBoard(Go19::Go19BoardWidth, Go19::Go19BoardHeight);
	players = ref new Vector<ygcPlayer^>();

	matchRules = ref new ygcRules();
	for (auto i = 0; Go19::defaultMoveValidators[i]; ++i)
		matchRules->validMoves.push_back(Go19::defaultMoveValidators[i]);
	
	for (auto i = 0; Go19::defaultPostMoveActions[i]; ++i)
		matchRules->postMoves.push_back(Go19::defaultPostMoveActions[i]);

	turn = ref new Go19::Go19StoneColor(Go19::Go19StoneColor::BLACK);

	moveHistory = ref new Vector<ygcMove^>();
}


/* ygcPlayer method implementation */

ygcPlayer::ygcPlayer(ygcStoneColor^ stonecolor)
{
	initPlayer(ygcPlayerInputType::DUMMY);

	this->color = stonecolor;
}

ygcPlayer::ygcPlayer(ygcStoneColor^ stonecolor, ygcPlayerInputType ypiType)
{
	initPlayer(ypiType);

	this->color = stonecolor;
}

ygcPlayer::ygcPlayer(ygcStoneColor^ stonecolor, ygcPlayerInputType ypiType, Platform::String^ name)
{
	initPlayer(ypiType);

	this->name = name;
	this->color = stonecolor;
}

void ygcPlayer::initPlayer(ygcPlayerInputType ypiType)
{
	name = "Player";
	stonesTaken = 0;
	ready = false;
	passed = false;

	switch (ypiType) {
	case SCREEN:
		inputHandler = ref new PlayerInput::ScreenInput();
		break;
	default:
		break;
	}
}

bool ygcPlayer::takeMove(uint16_t x, uint16_t y)
{
	ygcField^ field = (ygcField^)s;
	Vector<ygcPlayer^>^ pvec = this->currentMatch->players;
	Go19::Go19StoneColor^ gsc = static_cast<Go19::Go19StoneColor^>(this->currentMatch->turn);

	


	inputHandler->makeMove(b, x, y);
}