#include "stdafx.h"

#include "..\src\pass_one.h"
#include "..\src\pass_two.h"
#include "..\src\parser.h"
#include "..\src\spasm.h"
#include "..\src\errors.h"
#include "..\src\storage.h"

using namespace System;
using namespace System::Text;
using namespace System::Collections::Generic;
using namespace	Microsoft::VisualStudio::TestTools::UnitTesting;

namespace SPASMTestsVS2008
{
	[TestClass]
	public ref class AssembleTests
	{
	public: 
		
		[TestMethod]
		void directivemacro()
		{
			BYTE Results[] = {'h', 'e', 'l', 'l', 'o', 0x00};
			output_filename = strdup("test.bin");
			mode = MODE_NORMAL;
			init_storage();
			output_contents = (unsigned char *) malloc(1000);
			curr_input_file = strdup("..\\..\\..\\..\\..\\Tests\\SPASMTests\\directivemacro.z80");
			int result = run_assembly();

			for (int i = 0; i < sizeof(Results); i++)
			{
				Assert::AreEqual(Results[i], output_contents[i], gcnew String("Unequal at index: ") + i);
			}
		}

		[TestMethod]
		void echofill()
		{
			BYTE Results[] = {'A', 'Z', 'Z', 'Z', 'Z'};

			output_filename = strdup("test.bin");
			mode = MODE_NORMAL;
			init_storage();
			output_contents = (unsigned char *) malloc(1000);
			curr_input_file = strdup("..\\..\\..\\..\\..\\Tests\\SPASMTests\\echofill.z80");
			int result = run_assembly();

			for (int i = 0; i < sizeof(Results); i++)
			{
				Assert::AreEqual(Results[i], output_contents[i], gcnew String("Unequal at index: ") + i);
			}
		}
	};
}
