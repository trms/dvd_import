// MultiplexerGlue.h

#pragma once

using namespace System;

namespace Utilities {
	namespace DVDImport {

		public ref class MultiplexGlue
		{
		public:
			int Multiplex(System::String ^video, System::String ^audio, int offset, System::String ^output);
		};
	}
}