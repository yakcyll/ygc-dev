/*
* ygcGo19.cpp
* default (Tromp-Taylor, bar superko) rules implementation
*/

#include "pch.h"

using namespace ygc;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;

Go19::Go19Match::Go19Match(uint16_t width, uint16_t height) : koPoint(-1, -1)
{
	board = ref new ygcBoard(width, height);
	board->moveHistory = moveHistory;

	for (auto i = 0; Go19::defaultMoveValidators[i]; ++i)
		matchRules->validMoves.push_back(Go19::defaultMoveValidators[i]);

	for (auto i = 0; Go19::defaultPostMoveActions[i]; ++i)
		matchRules->postMoves.push_back(Go19::defaultPostMoveActions[i]);

	turn = ref new Go19::Go19StoneColor(Go19::Go19StoneColor::BLACK);
}

bool VectorSetAdd(Vector<Point>^ vec, Point value)
{
	uint32_t index = 0, tmp;
	if (!vec->IndexOf(value, &tmp)) {
		for (auto i = vec->First(); i->HasCurrent; i->MoveNext()) {
			if (i->Current.X > value.X || (i->Current.X == value.X && i->Current.Y > value.Y)) {
				vec->InsertAt(index, value);
				return true;
			}
			++index;
		}
		vec->Append(value);
		return true;
	}
	return false;
}

bool VectorSetAdd(Vector<Point>^ vec1, Vector<Point>^ vec2)
{
	for (auto i : vec2)
		VectorSetAdd(vec1, i);

	return true;
}

Vector<Point>^ getNeighbors(Point size, Point coord)
{
	Vector<Point>^ neighbors = ref new Vector<Point>();

	if (coord.X > 0) neighbors->Append(Point(coord.X - 1, coord.Y));
	if (coord.X < size.X - 1) neighbors->Append(Point(coord.X + 1, coord.Y));
	if (coord.Y > 0) neighbors->Append(Point(coord.X, coord.Y - 1));
	if (coord.Y < size.Y - 1) neighbors->Append(Point(coord.X, coord.Y + 1));

	return neighbors;
}

Vector<Point>^ getLiberties(ygcBoard^ b, Point coord)
{
	Vector<Point>^ liberties = ref new Vector<Point>();

	Go19::Go19StoneColor^ myColor = ref new Go19::Go19StoneColor(*(b->GetAt(coord)));
	Go19::Go19StoneColor^ floodfillColor = ref new Go19::Go19StoneColor(myColor);
	floodfillColor->increment();
	*b->GetAt(coord) = *floodfillColor;

	Vector<Point>^ neighbors = getNeighbors(Point(b->sBoardWidth, b->sBoardHeight), coord);
	for (Point neighbor : neighbors) {
		if (*b->GetAt(neighbor) == *myColor)
			VectorSetAdd(liberties, getLiberties(b, neighbor));

		else if (*b->GetAt(neighbor) == 0)
			VectorSetAdd(liberties, neighbor);
	}

	return liberties;
}

bool inAtari(ygcBoard^ board, Point coord)
{
	ygcBoard^ b = ref new ygcBoard(board);
	return getLiberties(b, coord)->Size == 1;
}

bool Go19::bGo19_IsLegal(ygcMatch^ m, Point coord)
{
	Go19::Go19StoneColor^ oc = ref new Go19::Go19StoneColor(*m->turn);
	oc->increment();

	if (*m->board->GetAt(coord) != Go19::Go19StoneColor::EMPTY)
		return false;

	if (coord.Equals(((Go19Match^)m)->koPoint))
		return false;

	bool suicide = true;
	for (Point neighbor : getNeighbors(Point(m->board->sBoardWidth, m->board->sBoardHeight), coord)) {
		if (*m->board->GetAt(neighbor) == 0)
			suicide = false;
		else if (*m->board->GetAt(neighbor) == *m->turn) {
			if (!inAtari(m->board, neighbor))
				suicide = false;
		}
		else if (*m->board->GetAt(neighbor) == *oc)
			if (inAtari(m->board, neighbor))
				suicide = false;
	}

	if (suicide)
		return false;

	return true;
}


Vector<Point>^ getChainPoints(ygcBoard^ b, Point coord)
{
	Vector<Point>^ stones = ref new Vector<Point>();
	stones->Append(coord);

	Go19::Go19StoneColor^ myColor = ref new Go19::Go19StoneColor(*b->GetAt(coord));
	Go19::Go19StoneColor^ floodfillColor = ref new Go19::Go19StoneColor(myColor);
	floodfillColor->increment();
	*b->GetAt(coord) = *floodfillColor;

	Vector<Point>^ neighbors = getNeighbors(Point(b->sBoardWidth, b->sBoardHeight), coord);

	for (Point neighbor : neighbors) {
		if (*b->GetAt(coord) == *myColor) {
			if (!stones->IndexOf(neighbor, nullptr))
				VectorSetAdd(stones, getChainPoints(b, neighbor));
		}
	}

	return stones;
}

uint16_t Go19::vGo19_SearchForPrisoners(ygcMatch^ m, Point coord)
{
	ygcBoard^ b = ref new ygcBoard(m->board);
	Vector<Point>^ stones;
	uint16_t cap_count = 0;

	Go19::Go19StoneColor^ oc = ref new Go19::Go19StoneColor(*m->turn);
	oc->increment();

	for (Point neighbor : getNeighbors(Point(m->board->sBoardWidth, m->board->sBoardHeight), coord)) {
		if (*m->board->GetAt(neighbor) == *oc) {
			if (inAtari(m->board, neighbor)) {
				stones = getChainPoints(b, neighbor);

				for (Point p : stones) {
					*m->board->GetAt(coord) = 0;
					++cap_count;
				}
			}
		}
	}

	if (cap_count == 1)
		((Go19Match^)m)->koPoint = stones->GetAt(0);
	else
		((Go19Match^)m)->koPoint = Point(-1, -1);

	return cap_count;
}