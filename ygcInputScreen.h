/*
* ygcInputScreen.h
* screen input handler header file
*/

#pragma once

#include "ygc.h"

using namespace Windows::UI::Xaml;

namespace ygc {

	namespace PlayerInput {

		private ref class ScreenInput : public ygcPlayerInput {
		private:
			static uint16_t screenPlayers;
			~ScreenInput();
		internal:
			ScreenInput();

			bool handleInput(Point) override;
		};

	};

};