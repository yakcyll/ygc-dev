/*
* ygcInputScreen.cpp
* screen input handler implementation
*/

#include "pch.h"

using namespace ygc;
using namespace ygc::PlayerInput;
using namespace Windows::Foundation;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Media::Imaging;

uint16_t ScreenInput::screenPlayers = 0;

ScreenInput::ScreenInput()
{
	++screenPlayers;
}

ScreenInput::~ScreenInput()
{
	--screenPlayers;
}

bool ScreenInput::handleInput(Object^ s, RoutedEventArgs^ e)
{
	static uint16_t eventsThisMove = 0;
	++eventsThisMove;

	if (eventsThisMove != screenPlayers)
		return false;

	eventsThisMove = 0;

	Canvas^ c = (Canvas^)s;
	TappedRoutedEventArgs^ te = (TappedRoutedEventArgs^)e;
	Point tapPoint = te->GetPosition(c);

	ygcBoard^ board = player->activeMatch->board;
	float sideMargin = float(c->Width > c->Height ? c->Width / float(board->sBoardWidth + 1) : c->Height / float(board->sBoardHeight + 1));

	if (tapPoint.X < sideMargin / 2.0f || tapPoint.X > c->Width - sideMargin / 2.0f ||
		tapPoint.Y < sideMargin / 2.0f || tapPoint.Y > c->Height - sideMargin / 2.0f)
		return false;

	uint16_t x = uint16_t((tapPoint.X - sideMargin / 2.0f) / sideMargin);
	uint16_t y = uint16_t((tapPoint.Y - sideMargin / 2.0f) / sideMargin);

	if (player->activeMatch->matchMakeMove(x, y))
		return true;

	return false;
}