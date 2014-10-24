/*
 * ygc.h
 * ygc representation file
 */

#pragma once

using namespace Platform;
using namespace Platform::Collections;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::System::Threading;

#include <cstdint>
#include <list>


namespace ygc {

	ref class ygcBoard;
	ref class ygcPlayer;
	ref class ygcMatch;

	typedef bool(*moveValidator)(ygcBoard^, uint16_t, uint16_t);
	typedef void(*postMoveAction)(ygcMatch^, uint16_t, uint16_t);

	enum ygcPlayerInputType { DUMMY, SCREEN, WIFI, BT, IGS, AI, SPEC };
	enum ygcStoneStatus { ADDED, FALLEN };
	enum ygcMatchState { ACTIVE, ARCHIVAL };
	
	template<typename T1, typename T2>
	private ref class pair sealed {

	public:
		property T1 first;
		property T2 second;
		pair() : first(), second() { }
		pair(T1 t1, T2 t2) : first(t1), second(t2) { }
	};

	private ref class ygcStoneColor {
	internal:
		inline void operator=(const ygcStoneColor %sc) { this->stoneColor = sc.stoneColor; this->maxval = sc.maxval; }
		inline void operator=(const int &i) { if (maxval) this->stoneColor = i % maxval; }
		inline bool operator==(const int &i) { if (i == stoneColor) return true; return false; }
		inline ygcStoneColor operator+(const int &i) { return ygcStoneColor((this->stoneColor + i) % maxval, maxval); }
		inline ygcStoneColor% operator+=(const int &i) { this->stoneColor = (this->stoneColor + i) % maxval; return *this; }
		inline ygcStoneColor operator-(const int &i) { return ygcStoneColor(((this->stoneColor - i) < 0 ? (this->stoneColor - i) + maxval : (this->stoneColor - i)), maxval); }

		uint16_t stoneColor;
		uint16_t maxval;

		ygcStoneColor() : maxval(0) {}
		ygcStoneColor(const uint16_t &i, const uint16_t &j) { this->stoneColor = i; maxval = j; }
		ygcStoneColor(const ygcStoneColor %y) : stoneColor(y.stoneColor), maxval(y.maxval) {}
		operator int() { return stoneColor; }

		void increment(unsigned int count = 1) { this->stoneColor += count; this->stoneColor %= maxval; if (this->stoneColor == 0) this->stoneColor++; }
		ygcStoneColor previous(unsigned int count = 1) { return ygcStoneColor(((this->stoneColor - count) < 1 ? (this->stoneColor - count) + maxval - 1: (this->stoneColor - count)), maxval); }
	};

	private ref class ygcStoneChange {
	internal:
		ygcStoneColor^ whose;
		ygcStoneStatus status;
		uint16_t x;
		uint16_t y;

		ygcStoneChange(ygcStoneColor^ w, ygcStoneStatus s, uint16_t x1, uint16_t y1) : whose(w), status(s), x(x1), y(y1) {}
	};

	private ref class ygcMove sealed {
	internal:
		Vector<ygcStoneChange^> stonesChanged;
	};

	private ref class ygcField sealed {
	internal:
		ygcStoneColor^ takenBy;
		uint16_t x;
		uint16_t y;

	};

	private ref class ygcBoard sealed {
	internal:
		uint16_t sBoardWidth;
		uint16_t sBoardHeight;

		Array<Object^>^ fields;
		Vector<ygcMove^>^ moveHistory;

		ygcBoard(uint16_t, uint16_t);
		ygcField^& getAt(uint16_t, uint16_t);
	};

	private ref class ygcPlayerInput abstract {
	internal:
		ygcPlayerInputType ypiType;
		ygcPlayer^ player;

		virtual bool handleInput(Object^, RoutedEventArgs^) = 0;
	};

	private ref class ygcPlayer sealed {
	internal:
		String^ name;
		ygcStoneColor^ color;
		uint32_t stonesTaken;

		bool ready;
		bool passed;

		ygcPlayerInput^ inputHandler;

		ygcMatch^ activeMatch;

		ygcPlayer(ygcMatch^, ygcStoneColor^);
		ygcPlayer(ygcMatch^, ygcStoneColor^, ygcPlayerInputType);
		ygcPlayer(ygcMatch^, ygcStoneColor^, ygcPlayerInputType, String^);

		void initPlayer(ygcMatch^, ygcPlayerInputType);
	};

	private ref class ygcRules sealed {

	internal:
		std::list<moveValidator> validMoves;
		std::list<postMoveAction> postMoves;
	};

	private ref class ygcMatch {

	internal:
		ygcBoard^ board;
		Vector<ygcPlayer^>^ players;	// allowing for a custom number of players would cause an 
										// unpleasant increase in sophistication of game logic; 
										// suffer at your own discretion

		ygcRules^ matchRules;

		ygcMatchState matchState;
		uint16_t moveid;
		Vector<ygcMove^>^ moveHistory;
		ygcStoneColor^ turn;			// this might be superficial; TODO: think about it when your eyelids won't be so damn heavy

		ygcMatch();
		
		bool matchMoveBack(unsigned int);
		bool matchMoveForward(unsigned int);
		bool matchMakeMove(uint16_t, uint16_t);

	};

};