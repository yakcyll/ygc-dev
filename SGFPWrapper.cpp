/*
* SGFPWrapper.h
* ygc SGF parser C++/CX wrapper source file
*/

#include "pch.h"
#include "SGFPWrapper.h"

using namespace ygc;

SGFPWrapper::SGFPWrapper() 
{
	sgfp = std::shared_ptr<SGFParser>(new SGFParser());
}

SGFPWrapper::SGFPWrapper(Platform::String^ fb) 
{
	std::wstring ws(fb->Begin());
	std::string as(ws.begin(), ws.end());
	sgfp = std::shared_ptr<SGFParser>(new SGFParser(as));
}

bool SGFPWrapper::ParseBuffer(Platform::String^ hps)
{
	if (hps) {
		std::wstring ws(hps->Begin());
		std::string as(ws.begin(), ws.end());
		return sgfp->ParseBuffer(as);
	}
	else {
		return sgfp->ParseBuffer(std::string());
	}
}

bool SGFPWrapper::ParseTree(void)
{
	return sgfp->ParseTree();
}

Platform::String^ SGFPWrapper::LoadSGFFile(Platform::String^ filepath)
{
	using namespace Windows::Storage;

	Platform::Array<byte>^ data = nullptr;
	std::wstring ws = filepath->Data();
	std::string path = std::string(ws.begin(), ws.end());
	std::wstring output;

	std::ifstream file(path, std::ios::in | std::ios::ate);
	if (file.is_open())
	{
		int length = (int)file.tellg();
		data = ref new Platform::Array<byte>(length);
		file.seekg(0, std::ios::beg);
		file.read(reinterpret_cast<char*>(data->Data), length);
		file.close();

		for (unsigned i = 0; i < data->Length; ++i)
			output += data[i];
	}

	return ref new Platform::String(output.c_str());
}
