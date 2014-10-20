/*
* ygcInputScreen.cpp
* screen input handler implementation
*/

#include "pch.h"

using namespace ygc;
using namespace ygc::PlayerInput;

bool ScreenInput::takeMove(uint16_t x, uint16_t y)
{

}

void ScreenInput::chatter(Platform::String^)
{

}

void ScreenInput::signalMove(uint16_t x, uint16_t y)
{
	coords.first = x;
	coords.second = y;
}