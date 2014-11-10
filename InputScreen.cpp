/*
* InputScreen.cpp
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

bool ScreenInput::handleInput(Point coord)
{
	static uint16_t eventsThisMove = 0;
	++eventsThisMove;

	if (eventsThisMove != screenPlayers)
		return false;

	eventsThisMove = 0;

	return player->activeMatch->matchMakeMove(coord);
}