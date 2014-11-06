/*
* ygc.h
* ygc representation implementation
*/

#include "pch.h"

using namespace ygc;
using namespace Windows::Foundation;

/* ygcMatch method implementation */

ygcMatch::ygcMatch() : board(nullptr), turn(nullptr), passCount(0), moveId(0), matchState(ygcMatchState::ACTIVE)
{
	players = ref new Vector<ygcPlayer^>();

	matchRules = ref new ygcRules();

	moveHistory = ref new Vector<ygcMove^>();
}

bool ygcMatch::matchMoveBack(uint16_t count)
{
	--moveId;
	ygcMove^ lastMove = moveHistory->GetAt(moveId);
	for (auto i = lastMove->stonesChanged.First(); i->HasCurrent; i->MoveNext()) {
		uint16_t playerIndex = (int)i->Current->whose - 1;

		if (i->Current->status == ygcStoneStatus::ADDED) {
			*board->GetAt(i->Current->coord) = 0;
		}
		else if (i->Current->status == ygcStoneStatus::FALLEN) {
			*board->GetAt(i->Current->coord) = i->Current->whose;
		}
	}

	return true;
}

bool ygcMatch::matchMoveForward(uint16_t count)
{
	ygcMove^ lastMove = moveHistory->GetAt(moveId);
	for (auto i = lastMove->stonesChanged.First(); i->HasCurrent; i->MoveNext()) {
		uint16_t playerIndex = (int)i->Current->whose - 1;

		if (i->Current->status == ygcStoneStatus::ADDED) {
			*board->GetAt(i->Current->coord) = i->Current->whose;
		}
		else if (i->Current->status == ygcStoneStatus::FALLEN) {
			*board->GetAt(i->Current->coord) = 0;
		}
	}
	++moveId;

	return true;
}

bool ygcMatch::matchMakeMove(Point coord)
{
	for (auto f : matchRules->validMoves)
		if (!f(this, coord))
			return false;

	ygcMove^ move = ref new ygcMove();
	Vector<Point>^ stonesChanged = nullptr;

	if (moveHistory->Size != moveId) {
		while (moveHistory->Size != moveId)
			moveHistory->RemoveAtEnd(); // remove one prior to last one
	}

	move->stonesChanged.Append(ref new ygcStoneChange(*this->turn, ygcStoneStatus::ADDED, coord));
	moveHistory->Append(move);

	*board->GetAt(coord) = *turn;

	for (auto f : matchRules->postMoves)
		stonesChanged = f(this, coord); // TODO: probably should be +=, maybe VectorSetAdd?

	if (stonesChanged) {
		for (unsigned i = 0; i < stonesChanged->Size; ++i) {
			Point p = stonesChanged->GetAt(i);
			move->stonesChanged.Append(ref new ygcStoneChange(*board->GetAt(p), ygcStoneStatus::FALLEN, p));
			*board->fields->GetAt((uint16_t)p.Y * board->sBoardWidth + (uint16_t)p.X) = 0; // TODO: default action - stone removal?
		}

		players->GetAt((int)turn - 1)->score += stonesChanged->Size;
	}

	turn->increment();
	passCount = 0;
	moveId++;

	return true;

}

bool ygcMatch::matchSkipTurn(uint16_t count)
{
	if (count >= players->Size)
		return false;

	while (count--) {
		++passCount;
		turn->increment();
	}

	return passCount < players->Size;
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
