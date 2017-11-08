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
	TEST_CLASS(ParserTests)
	{
	public:
		TEST_METHOD_INITIALIZE(init)
		{
			ClearSPASMErrorSessions();
			session = StartSPASMErrorSession();
			output_contents = (unsigned char *)malloc(output_buf_size);
			init_storage();
			char cwd[256];
			getcwd(cwd, sizeof(cwd));
			curr_input_file = _strdup("..\\..\\SPASMNativeTests\\testfiles\\Tests.asm");
			output_filename = "output.bin";
			mode = MODE_NORMAL;
			int nResult = run_assembly();
			DWORD dwLastError = GetLastSPASMError();
			Assert::AreEqual((int)EXIT_NORMAL, nResult, TEXT("Could not open test file"));
		}

		TEST_METHOD(Math1)
		{
			bool success;
			int result;
			success = parse_num("1+1", &result);
			Assert::IsTrue(success);
			Assert::AreEqual(2, result);
		}
		TEST_METHOD(Math2)
		{

			bool success;
			int result;
			success = parse_num("((1+1)*2+3)/2)*4+3", &result);
			Assert::IsTrue(success);
			Assert::AreEqual(15, result);
		};
		TEST_METHOD(MathOoO)
		{
			bool success;
			int result;
			success = parse_num("1+2/2", &result);
			Assert::IsTrue(success);
			Assert::AreEqual(1, result);
		};
		TEST_METHOD(Math3)
		{
			bool success;
			int result;
			success = parse_num("(1+1)+(5%2)+(4/4)+(2^1)", &result);
			Assert::IsTrue(success);
			Assert::AreEqual(7, result);
		};
		TEST_METHOD(BadMath1)
		{
			bool success;
			int result;
			success = parse_num("1+1(", &result);
			Assert::IsFalse(success);
		};
		TEST_METHOD(BadMath2)
		{
			bool success;
			int result;
			success = parse_num("1+made_up_label", &result);
			Assert::IsFalse(success);
		};
		TEST_METHOD (Whitespace)
		{
			bool success;
			int result;
			success = parse_num("1+1     +1\t+1  +1+ 1\t\t+1", &result);
			Assert::IsTrue(success);
			Assert::AreEqual(7, result);
		};
		TEST_METHOD(Ascii)
		{
			bool success;
			int result;
			success = parse_num("'a'+'b'", &result);
			Assert::IsTrue(success);
			Assert::AreEqual('a' + 'b', result);
		};

		TEST_METHOD(ParserError103)
		{
			bool success;
			int result;
			success = parse_num("1 1", &result);
			Assert::IsFalse(success);
			Assert::AreEqual((DWORD)SPASM_ERR_OPERATOR_EXPECTED, GetLastSPASMError());
		};

		TEST_METHOD(ParserError105)
		{
			bool success;
			int result;
			success = parse_num("1 + ^1", &result);
			Assert::IsFalse(success);
			Assert::AreEqual((DWORD)SPASM_ERR_BAD_VALUE_PREFIX, GetLastSPASMError());
		};

		TEST_METHOD(ParserError102)
		{
			bool success;
			int result;
			pass_one = true;
			success = parse_num("ParserError102()", &result);
			Assert::IsFalse(success);
			Assert::AreEqual((DWORD)SPASM_ERR_ARG_USED_WITHOUT_VALUE, GetLastSPASMError());
		};

		TEST_METHOD(ParserError200)
		{
			bool success;
			int result;
			pass_one = true;
			success = parse_num("20a", &result);
			Assert::IsFalse(success);
			Assert::AreEqual((DWORD)SPASM_ERR_INVALID_DECIMAL_DIGIT, GetLastSPASMError());
		};

		TEST_METHOD(ParserError201)
		{
			bool success;
			int result;
			pass_one = true;
			success = parse_num("$20ag", &result);
			Assert::IsFalse(success);
			Assert::AreEqual((DWORD)SPASM_ERR_INVALID_HEX_DIGIT, GetLastSPASMError());
		};

		TEST_METHOD(ParserError202)
		{
			bool success;
			int result;
			pass_one = true;
			success = parse_num("%10120", &result);
			Assert::IsFalse(success);
			Assert::AreEqual((DWORD)SPASM_ERR_INVALID_BINARY_DIGIT, GetLastSPASMError());
		};

		TEST_METHOD(ParserError115)
		{
			bool success;
			int result;
			pass_one = true;
			success = parse_num("InfiniteRecursiveDefine", &result);
			Assert::IsFalse(success);
			Assert::AreEqual((DWORD)SPASM_ERR_EXCEEDED_RECURSION_LIMIT, GetLastSPASMError());
		};
	private:
		int session;
	};
}