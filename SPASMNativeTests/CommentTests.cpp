#include "..\src\stdafx.h"
#include "CppUnitTest.h"

#include <string>
#include <fstream>
#include <streambuf>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
#include "..\src\pass_one.h"
#include "..\src\pass_two.h"
#include "..\src\parser.h"
#include "..\src\spasm.h"
#include "..\src\errors.h"
#include "..\src\storage.h"

namespace SPASMNativeTests
{
	TEST_CLASS(PreprocessorTests)
	{
	public:
		int session;

		TEST_METHOD_INITIALIZE(init)
		{
			ClearSPASMErrorSessions();
			session = StartSPASMErrorSession();
			output_contents = (unsigned char *)malloc(output_buf_size);
			init_storage();

			output_filename = "output.bin";
			mode = MODE_NORMAL;
		}

		TEST_METHOD_CLEANUP(cleanup)
		{
			free_storage();
			free(output_contents);
		}

		std::string read_contents(const std::string &filename) {
			std::ifstream t(filename);
			std::string str((std::istreambuf_iterator<char>(t)),
				std::istreambuf_iterator<char>());
			return str;
		}

		TEST_METHOD(WeirdParenInComment)
		{
			curr_input_file = _strdup("..\\..\\SPASMNativeTests\\testfiles\\commentwithparen.z80");
			int nResult = run_assembly();
			Assert::AreEqual(0, nResult);
		}

	};
}
