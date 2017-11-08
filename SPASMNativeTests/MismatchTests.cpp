#include "..\src\stdafx.h"
#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
#include "..\src\pass_one.h"
#include "..\src\pass_two.h"
#include "..\src\parser.h"
#include "..\src\spasm.h"
#include "..\src\errors.h"
#include "..\src\storage.h"

namespace SPASMNativeTests
{
	TEST_CLASS(MismatchTests)
	{
	public:
		// Run the file, return the error code
		int RunTest(const char *file_name)
		{
			ClearSPASMErrorSessions();
			char full_filename[256] = "..\\..\\SPASMNativeTests\\testfiles\\";
			strcat_s(full_filename, file_name);

			output_contents = (unsigned char *)malloc(output_buf_size);
			init_storage();
			curr_input_file = _strdup(full_filename);
			output_filename = "output.bin";
			mode = MODE_LIST;

			run_assembly();

			mode = 0;
			free_storage();
			free(output_contents);

			return (int)GetLastSPASMError();
		}

		TEST_METHOD(Mismatch1)
		{
			RunTest("mismatch1.z80");
			Assert::AreEqual(SPASM_ERR_UNMATCHED_IF, (int)GetLastSPASMError(), TEXT("Wrong error code"));
			Assert::AreEqual(1, GetLastSPASMErrorLine(), TEXT("Wrong error line"));
		}

		TEST_METHOD(Mismatch2)
		{
			RunTest("mismatch2.z80");
			Assert::AreEqual(SPASM_ERR_UNMATCHED_IF, (int)GetLastSPASMError(), TEXT("Wrong error code"));
			Assert::AreEqual(5, GetLastSPASMErrorLine(), TEXT("Wrong error line"));
		}
		TEST_METHOD(Mismatch3)
		{
			Assert::AreEqual(SPASM_ERR_UNMATCHED_IF, RunTest("mismatch3.z80"));
		}
		TEST_METHOD(Mismatch4)
		{
			Assert::AreEqual(SPASM_ERR_UNMATCHED_IF, RunTest("mismatch4.z80"));
		}
		TEST_METHOD(Mismatch5)
		{
			Assert::AreEqual(SPASM_ERR_SUCCESS, RunTest("mismatch5.z80"));
		}
	};
}