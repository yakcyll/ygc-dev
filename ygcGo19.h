/*
* ygcGo19.h
* default rules header file
*/

#pragma once

#include "ygc.h"

namespace ygc {

	namespace Go19 {

		private ref class Go19StoneColor sealed : public ygcStoneColor { 

		internal:
			const static uint16_t EMPTY = 0;
			const static uint16_t BLACK = 1;
			const static uint16_t WHITE = 2;

			Go19StoneColor() { stoneColor = EMPTY; maxval = 3; };
			Go19StoneColor(uint16_t sc) { maxval = 3; stoneColor = sc % maxval; };
			
		};

		const uint16_t Go19PlayerCount = 2;

		const uint16_t Go19BoardHeight = 9;
		const uint16_t Go19BoardWidth = 6;

		bool bGo19_IsEmpty(ygcBoard^, uint16_t, uint16_t);
		bool bGo19_HasLiberties(ygcBoard^, uint16_t, uint16_t);
		void vGo19_SearchForPrisoners(ygcMatch^, uint16_t, uint16_t);

		const static moveValidator defaultMoveValidators[3] = { bGo19_IsEmpty, bGo19_HasLiberties, NULL };
		const static postMoveAction defaultPostMoveActions[2] = { vGo19_SearchForPrisoners, NULL };

		private ref class Go19Match : public ygcMatch {

		internal:
			Go19Match();
		};

	};

};