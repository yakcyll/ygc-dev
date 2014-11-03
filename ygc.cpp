/*
* ygc.h
* ygc representation implementation
*/

#include "pch.h"

using namespace ygc;
using namespace Windows::Foundation;

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

bool ygcMatch::matchMakeMove(Point coord)
{
	for (auto f : matchRules->validMoves)
		if (!f(this, coord))
			return false;

	ygcMove^ move = ref new ygcMove();
	move->stonesChanged.Append(ref new ygcStoneChange(this->turn, ygcStoneStatus::ADDED, (uint16_t)coord.X, (uint16_t)coord.Y));
	moveHistory->Append(move);

	*board->GetAt(coord) = *turn;

	for (auto f : matchRules->postMoves)
		players->GetAt((int)*turn - 1)->score += f(this, coord);

	turn->increment();

	moveid++;

	return true;
}

/* ygcBoard method implementation */

ygcBoard::ygcBoard(uint16_t sbw, uint16_t sbh) : sBoardWidth(sbw), sBoardHeight(sbh), moveHistory(nullptr)
{
	fields = ref new Vector<ygcStoneColor^>(sBoardWidth * sBoardHeight);
	for (uint16_t index = 0; index < fields->Size; ++index)
		fields->SetAt(index, ref new ygcStoneColor());
}

ygcBoard::ygcBoard(const ygcBoard^ b) : sBoardWidth(b->sBoardWidth), sBoardHeight(b->sBoardHeight), moveHistory(nullptr)
{
	fields = ref new Vector<ygcStoneColor^>(sBoardWidth * sBoardHeight);
	for (uint16_t index = 0; index < fields->Size; ++index)
		fields->SetAt(index, ref new ygcStoneColor(*b->fields->GetAt(index)));
}

ygcStoneColor^ ygcBoard::GetAt(Point coord) 
{
	ygcStoneColor^ rp = fields->GetAt((uint16_t)coord.Y * sBoardWidth + (uint16_t)coord.X);
	return rp;
}

void ygcBoard::SetAt(Point coord, ygcStoneColor sc)
{
	*fields->GetAt((uint16_t)coord.Y * sBoardWidth + (uint16_t)coord.X) = sc;
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
	score = 0.0;

	switch (ypiType) {
	case SCREEN:
		inputHandler = ref new PlayerInput::ScreenInput();
		ready = true;
		break;
	default:
		break;
	}

	inputHandler->ypiType = ypiType;
	inputHandler->player = this;
}
