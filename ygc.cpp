/*
* ygc.h
* ygc representation implementation
*/

#include "pch.h"

using namespace ygc;


/* ygcMatch method implementation */

ygcMatch::ygcMatch() : board(nullptr), turn(nullptr)
{
	players = ref new Vector<ygcPlayer^>();

	matchRules = ref new ygcRules();

	matchState = ygcMatchState::ACTIVE;
	moveid = 0;
	moveHistory = ref new Vector<ygcMove^>();
}


bool ygcMatch::matchMoveBack(unsigned int count)
{
	return true;
}

bool ygcMatch::matchMoveForward(unsigned int count)
{
	return true;
}

bool ygcMatch::matchMakeMove(uint16_t x, uint16_t y)
{
	for (auto f : matchRules->validMoves)
		if (!f(board, x, y))
			return false;

	moveHistory->Append(ref new ygcMove());
	moveHistory->GetAt(moveHistory->Size - 1)->stonesChanged.Append(ref new ygcStoneChange(this->turn, ygcStoneStatus::ADDED, x, y));

	*board->GetAt(x,y)->takenBy = *turn;

	for (auto f : matchRules->postMoves)
		f(this, x, y);

	turn->increment();

	moveid++;

	return true;
}

/* ygcBoard method implementation */

ygcBoard::ygcBoard(uint16_t sbw, uint16_t sbh) : sBoardWidth(sbw), sBoardHeight(sbh), moveHistory(nullptr)
{
	fields = ref new Array<Object^>(sBoardWidth);
	for (auto i = 0; i < sBoardWidth; ++i) {
		fields[i] = ref new Array<ygcField^>(sBoardHeight);
		for (auto j = 0; j < sBoardHeight; ++j) {
			((Array<ygcField^>^)fields[i])[j] = ref new ygcField();
		}
	}
}

ygcField^& ygcBoard::GetAt(uint16_t x, uint16_t y) 
{
	return ((Array<ygcField^>^)fields[x])[y];
}

/* ygcPlayer method implementation */

ygcPlayer::ygcPlayer(ygcMatch^ currentMatch, ygcStoneColor^ stonecolor)
{
	initPlayer(currentMatch, ygcPlayerInputType::DUMMY);

	this->color = stonecolor;
}

ygcPlayer::ygcPlayer(ygcMatch^ currentMatch, ygcStoneColor^ stonecolor, ygcPlayerInputType ypiType)
{
	initPlayer(currentMatch, ypiType);

	this->color = stonecolor;
}

ygcPlayer::ygcPlayer(ygcMatch^ currentMatch, ygcStoneColor^ stonecolor, ygcPlayerInputType ypiType, Platform::String^ name)
{
	initPlayer(currentMatch, ypiType);

	this->name = name;
	this->color = stonecolor;
}

void ygcPlayer::initPlayer(ygcMatch^ currentMatch, ygcPlayerInputType ypiType)
{
	name = "Player";
	stonesTaken = 0;
	ready = false;
	passed = false;
	activeMatch = currentMatch;

	switch (ypiType) {
	case SCREEN:
		inputHandler = ref new PlayerInput::ScreenInput();
		ready = true;
		break;
	default:
		break;
	}

	inputHandler->player = this;
}
