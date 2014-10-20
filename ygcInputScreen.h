/*
* ygcInputScreen.h
* screen input handler header file
*/

#pragma once

#include "ygc.h"

namespace ygc {

	namespace PlayerInput {

		public ref class ScreenInput : public ygcPlayerInput {
		private:
			pair<uint16_t, uint16_t> coords;

		public:
			ScreenInput() { ypiType = ygcPlayerInputType::SCREEN; }

			bool takeMove(uint16_t, uint16_t) override;
			void chatter(Platform::String^) override;

			void signalMove(uint16_t, uint16_t);
		};

	};

};