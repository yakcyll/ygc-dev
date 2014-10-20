/*
 * ygc.h
 * ygc representation file
 */

#pragma once

using namespace Platform;
using namespace Platform::Collections;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::System::Threading;

#include <cstdint>
#include <list>
#include <vector>

namespace ygc {

	typedef bool(*moveValidator)(ygcBoard^, uint16_t, uint16_t);
	typedef void(*postMoveAction)(ygcBoard^, uint16_t, uint16_t);

	enum ygcPlayerInputType { DUMMY, SCREEN, WIFI, BT, IGS, AI, SPEC };
	enum ygcStoneStatus { ADDED, FALLEN };
	
	template<typename T1, typename T2>
	private ref class pair sealed {

	public:
		T1 first;
		T2 second;
		pair() : first(), second() { }
		pair(T1 t1, T2 t2) : first(t1), second(t2) { }
	};

	private ref class ygcField : public Button {
	public:
		property ygcStoneColor^ takenBy;
		property uint16_t x;
		property uint16_t y;

	};

	public ref class ygcBoard sealed {

	public:
		uint16_t sBoardWidth;
		uint16_t sBoardHeight;

		Array<Array<ygcField ^>^>^ fields;

		ygcBoard(uint16_t sbw, uint16_t sbh) : sBoardWidth(sbw), sBoardHeight(sbh) { 
			fields = ref new Array<Array<ygcField^>^>(sBoardWidth); 
			for (auto i = 0; i < sBoardWidth; ++i)
				fields[i] = ref new Array<ygcField^>(sBoardHeight);
		}
	};

	private ref class ygcStoneColor abstract {
	public:
		uint16_t stoneColor;
	protected:
		ygcStoneColor();
	};

	private ref class ygcPlayerInput abstract {
	private:

	public:
		ygcPlayerInputType ypiType;

		virtual bool takeMove(uint16_t x, uint16_t y) = 0;
		virtual void chatter(Platform::String^) = 0;

		ygcPlayerInput();
	};

	public ref class ygcPlayer sealed {

	public:
		String^ name;
		ygcStoneColor^ color;
		uint32_t stonesTaken;

		bool ready;
		bool passed;

		ygcPlayerInput^ inputHandler;

		ygcPlayer(ygcStoneColor^);
		ygcPlayer(ygcStoneColor^, ygcPlayerInputType);
		ygcPlayer(ygcStoneColor^, ygcPlayerInputType, String^);

		void initPlayer(ygcPlayerInputType);

		bool takeMove(ygcBoard^, uint16_t, uint16_t);
		void chatter(Platform::String^);
	};

	public ref class ygcStoneChange sealed {

	public:
		ygcStoneColor^ whose;
		ygcStoneStatus status;
		uint16_t x;
		uint16_t y;
	};

	public ref class ygcMove sealed {

	public:
		Vector<ygcStoneChange^> stonesChanged;
	};

	public ref class ygcRules sealed {

	public:
		std::list<moveValidator> validMoves;
		std::list<postMoveAction> postMoves;
	};

	public ref class ygcMatch sealed {

	public:
		ygcBoard^ board;
		Vector<ygcPlayer^>^ players;	// allowing for a custom number of players would cause an 
									// unpleasant increase in sophistication of game logic; 
									// suffer at your own discretion

		ygcRules^ matchRules;

		ygcStoneColor^ turn;
		Vector<ygcMove^>^ moveHistory;

		ygcMatch();							// default rules, i.e. Go on a 19x19 board

	};

};