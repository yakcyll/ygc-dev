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
	
		SGFPWrapper() { 
			sgfp = std::shared_ptr<SGFParser>(new SGFParser());  
		}

		SGFPWrapper(Platform::String^ fb) { 
			std::wstring ws(fb->Begin()); 
			std::string as(ws.begin(), ws.end()); 
			sgfp = std::shared_ptr<SGFParser>(new SGFParser(as)); 
		}

		bool ParseBuffer(Platform::String^ hps = nullptr)
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

		bool ParseTree(void)
		{
			return sgfp->ParseTree();
		}

		Platform::String^ LoadSGFFile(Platform::String^ filepath)
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

			/*Platform::String^ output = ref new Platform::String();

			concurrency::task<StorageFile^> readFile(StorageFile::GetFileFromPathAsync(filepath));
			readFile.then([this](StorageFile^ sgfFile) {
				return sgfFile->OpenReadAsync();
			}).then([this, &output](Streams::IRandomAccessStream^ textStream) {
				Streams::DataReader ^textReader = ref new Streams::DataReader(textStream);
				unsigned textLength = (unsigned)textStream->Size;
				//IAsyncOperation<unsigned int>^ taskLoad = textReader->LoadAsync(textLength);
				concurrency::create_task(textReader->LoadAsync(textLength)).wait();
				output = textReader->ReadString(textLength);
			}).wait();

			concurrency::create_task(Windows::Storage::FileIO::ReadTextAsync(sgfFile)).then(
			[this, &output](Platform::String^ rt) { 
				output = rt; 
			}).wait(); */

			return ref new Platform::String(output.c_str());
		}
	};

}