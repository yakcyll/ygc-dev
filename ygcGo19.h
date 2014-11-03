/*
* ygcGo19.h
* default (Tromp-Taylor, bar superko) rules header file
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

			Go19StoneColor() { stoneColor = EMPTY; maxval = 3; }
			Go19StoneColor(uint16_t sc) { maxval = 3; stoneColor = sc % maxval; }
			Go19StoneColor(const ygcStoneColor %sc) { maxval = 3; stoneColor = sc.stoneColor % maxval; }

			inline void operator=(const ygcStoneColor %sc) { this->stoneColor = sc.stoneColor % 3; this->maxval = 3; }
			inline void operator=(const Go19StoneColor %sc) { this->stoneColor = sc.stoneColor; this->maxval = 3; }

		};

		const uint16_t Go19PlayerCount = 2;

		const uint16_t Go19BoardHeight = 19;
		const uint16_t Go19BoardWidth = 19;

		bool bGo19_IsLegal(ygcMatch^, Point);
		uint16_t vGo19_SearchForPrisoners(ygcMatch^, Point);

		const static moveValidator defaultMoveValidators[2] = { bGo19_IsLegal, NULL };
		const static postMoveAction defaultPostMoveActions[2] = { vGo19_SearchForPrisoners, NULL };

		private ref class Go19Match : public ygcMatch {

		internal:
			Point koPoint;

			Go19Match(uint16_t width = Go19::Go19BoardWidth, uint16_t height = Go19::Go19BoardHeight);
		};

	};

};