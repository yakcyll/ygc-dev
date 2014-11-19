/*
* SGFPWrapper.h
* ygc SGF parser C++/CX wrapper header
*/

#include "pch.h"
#include <fstream>
#include <string>
#include "SGFParser.h"

namespace ygc {

	private ref class SGFPWrapper {
	internal:
		std::shared_ptr<SGFParser> sgfp;
	
		SGFPWrapper();
		SGFPWrapper(Platform::String^ fb);

		bool ParseBuffer(Platform::String^ = nullptr);
		bool ParseTree(void);
		Platform::String^ LoadSGFFile(Platform::String^);
	};

}