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

			curr_input_file = _strdup("..\\..\\SPASMNativeTests\\testfiles\\PreprocessorTests.z80");
			output_filename = "output.bin";
			mode = MODE_NORMAL;
			int nResult = run_assembly();
			DWORD dwLastError = GetLastSPASMError();
			Assert::AreEqual((int)EXIT_NORMAL, nResult, TEXT("Could not open test file"));
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

		void RunTest(const char *function_name)
		{
			char szFunctionName[256];
			strcpy_s(szFunctionName, strrchr(function_name, ':') + 1);

			char buffer[256];
			sprintf_s(buffer, " %s()", szFunctionName);

			char szFileName[256];
			sprintf_s(szFileName, "%s.txt", szFunctionName);
			add_define(strdup("OUTPUT_FILE"), NULL)->contents = strdup(szFileName);

			ClearSPASMErrorSessions();
			int session = StartSPASMErrorSession();
			run_first_pass(buffer);
			EndSPASMErrorSession(session);

			Assert::AreEqual(read_contents(szFileName).c_str(), "PASS");
		}

		TEST_METHOD(Ifdef)
		{
			RunTest(__FUNCTION__);
		}

		TEST_METHOD(Ifndef)
		{
			RunTest(__FUNCTION__);
		}

		TEST_METHOD(Else)
		{
			RunTest(__FUNCTION__);
		}

		TEST_METHOD(Else2)
		{
			RunTest(__FUNCTION__);
		}

		TEST_METHOD(Else3)
		{
			RunTest(__FUNCTION__);
		}

		TEST_METHOD(If)
		{
			RunTest(__FUNCTION__);
		}

		TEST_METHOD(Elif)
		{
			RunTest(__FUNCTION__);
		}

		TEST_METHOD(Elif2)
		{
			RunTest(__FUNCTION__);
		}

		TEST_METHOD(LessThan)
		{
			RunTest(__FUNCTION__);
		}

		TEST_METHOD(LessThanEqualTo)
		{
			RunTest(__FUNCTION__);
		}

		TEST_METHOD(DoubleEquals)
		{
			RunTest(__FUNCTION__);
		}

		TEST_METHOD(Equals)
		{
			RunTest(__FUNCTION__);
		}
	};
}
