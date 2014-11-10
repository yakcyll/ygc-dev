/*
* SGFParser.h
* ygc SGF parser header
*/

#include "ygc.h"

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

	};


}