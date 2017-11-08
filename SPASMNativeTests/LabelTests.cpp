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
	TEST_CLASS(LabelTests)
	{
	public:
		int session;

		TEST_METHOD_INITIALIZE(init)
		{
			ClearSPASMErrorSessions();
			session = StartSPASMErrorSession();
			output_contents = (unsigned char *)malloc(output_buf_size);
			init_storage();

			curr_input_file = _strdup("..\\..\\SPASMNativeTests\\testfiles\\Tests.asm");
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

		TEST_METHOD(LocalLabel1)
		{
			char buffer[256] = "LocalLabel1()";
			error_occurred = false;
			run_first_pass(buffer);
			Assert::IsFalse(error_occurred);

			bool success;
			int result;
			success = parse_num("-_", &result);
			Assert::IsTrue(success, TEXT("Failed to parse the local label"));
			Assert::AreEqual(3, result);
		};

		TEST_METHOD(LocalLabel2)
		{
			bool success;
			int result;

			pass_one = false;
			success = parse_num("_", &result);
			Assert::IsFalse(success, TEXT("Did not fail when it should have"));
			Assert::AreEqual((DWORD)SPASM_ERR_LOCAL_LABEL_FORWARD_REF, GetLastSPASMError());
		};

		TEST_METHOD(LocalLabel3)
		{
			char buffer[256] = "LocalLabel3()";
			error_occurred = false;
			run_first_pass(buffer);
			Assert::IsFalse(error_occurred);

			bool success;
			int result;
			success = parse_num("-_+--_", &result);
			Assert::IsTrue(success, TEXT("Failed to parse the local label"));
			Assert::AreEqual(8, result);
		};

		TEST_METHOD(LocalLabel4)
		{
			out_ptr = output_contents;

			char buffer[256] = "LocalLabel4()";
			error_occurred = false;
			run_first_pass(buffer);
			Assert::IsFalse(error_occurred);

			run_second_pass();

			Assert::AreEqual((unsigned char)7, output_contents[2]);
		};


		TEST_METHOD(Error104_org)
		{
			out_ptr = output_contents;

			char buffer[256] = ".org ";
			error_occurred = false;
			run_first_pass(buffer);
			Assert::AreEqual((DWORD)SPASM_ERR_VALUE_EXPECTED, GetLastSPASMError());
		};

		TEST_METHOD(Error107_org)
		{
			out_ptr = output_contents;

			char buffer[256] = ".org -1";
			error_occurred = false;
			run_first_pass(buffer);
			Assert::AreEqual((DWORD)SPASM_ERR_INVALID_ADDRESS, GetLastSPASMError());
		};
	};
}