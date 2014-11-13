/*
* SGFParser.h
* ygc SGF parser header
*/

#include <boost/spirit/include/classic_core.hpp>
#include "ygc.h"

using namespace BOOST_SPIRIT_CLASSIC_NS;

namespace ygc {

	private class FileHeader {
	public:
		Platform::String^ sourceApplication;
		Platform::String^ fileEncoding;
		uint8_t fileFormat;
		uint8_t gameMode;
	};

	private class GameMetaData {
	public:
		Platform::String^ name;
		Platform::String^ extrainfo;
		Platform::String^ location;
		Windows::Globalization::Calendar^ date;

		Platform::String^ atevent;
		Platform::String^ round;

		Platform::String^ source;
	};

	private class MatchInfo {
	public:
		Platform::String^ black;
		Platform::String^ blackRank;
		Platform::String^ white;
		Platform::String^ whiteRank;

		uint16_t size[2];

		Platform::String^ ruleset;
		Platform::String^ result;
		float komi;
		uint16_t timePerPlayer;
		uint16_t handicap;
	};

	public ref class SGFTreeNode sealed {
	internal:
		Platform::String^ nodeName;
		Map<Platform::String^, Platform::String^>^ properties;
		Vector<SGFTreeNode^>^ children;
	};

	private class SGFParser : public grammar < SGFParser > {
	public:
		Vector<SGFTreeNode^>^ games;

		template <typename ScannerT>
		struct definition {

			definition(SGFParser const &)
			{
				Collection = GameTree >> *GameTree;
				GameTree = "(" >> Sequence >> *GameTree >> ")";
				Sequence = Node >> *Node;
				Node = ";" >> *Property;
				Property = PropIdent >> PropValue >> *PropValue;
				PropIdent = UcLetter >> *UcLetter;
				PropValue = "[" >> CValueType >> "]";
				CValueType = (ValueT | Compose);
				ValueT = (None | Number | Real | Double | Color | SimpleText | Text | Stone);

				ListOf = PropValue >> *PropValue

				None = epsilon_p;
				Digit = digit_p;
				UcLetter = upper_p;
				Number = !("+" | "-") >> Digit >> *Digit;
				Real = Numer >> !("." >> Digit >> *Digit);
				Double = "1" | "2";
				Color = "B" | "W";
				SimpleText = Text - space_p;
				Text = print_p;
				Compose = ValueT >> ":" >> ValueT;

				Stone = alpha_p >> alpha_p;
			}

			rule<ScannerT> Collection, GameTree, Sequence, Node, Property, PropIdent, PropValue, CValueType, ValueT, None, Digit, UcLetter, Number, Real, Double, Color, SimpleText, Text, Compose, Stone;

			rule<ScannerT> const &
				start() const { return Collection; }

		};

		void add_game(char const*, char const*) { }
		void add_seq(char const*, char const*) { }
		void add_node(char const*, char const*) { }
		void add_prop(char const*, char const*) { }
		void add_root(char const*, char const*) { }

	};


}