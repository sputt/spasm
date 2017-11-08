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
	TEST_CLASS(MacroTests)
	{
	public:
		int session;

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


		TEST_METHOD_CLEANUP(cleanup)
		{
			free_storage();
			free(output_contents);
		}

		TEST_METHOD(Macro1)
		{
			bool success;
			int result;
			success = parse_num("Macro1()+1", &result);
			Assert::IsTrue(success);
			Assert::AreEqual(2, result);
		};

		TEST_METHOD(Macro2)
		{
			bool success;
			int result;
			success = parse_num("Macro1+1", &result);
			Assert::IsTrue(success);
			Assert::AreEqual(2, result);
		};

		TEST_METHOD(Macro3)
		{
			bool success;
			int result;
			success = parse_num("Macro1()+Macro1+Macro1( )+Macro1()", &result);
			Assert::IsTrue(success);
			Assert::AreEqual(4, result);
		};

		TEST_METHOD(RecursiveMacro1)
		{
			bool success;
			int result;
			success = parse_num("RecursiveMacro1(6)", &result);
			Assert::IsTrue(success);
			Assert::AreEqual(6, result);
		};

		TEST_METHOD(FibonacciMacro0)
		{
			bool success;
			int result;
			success = parse_num("FibonacciMacro(0)", &result);
			Assert::IsTrue(success);
			Assert::AreEqual(0, result);
		};

		TEST_METHOD(FibonacciMacro1)
		{
			bool success;
			int result;
			success = parse_num("FibonacciMacro(1)", &result);
			Assert::IsTrue(success);
			Assert::AreEqual(1, result);
		};

		TEST_METHOD(FibonacciMacro12)
		{
			bool success;
			int result;
			success = parse_num("FibonacciMacro(12)", &result);
			Assert::IsTrue(success);
			Assert::AreEqual(144, result);
		};

		std::string read_contents(const std::string &filename) {
			std::ifstream t(filename);
			std::string str((std::istreambuf_iterator<char>(t)),
				std::istreambuf_iterator<char>());
			return str;
		}

		TEST_METHOD(EchoMacro1)
		{
			char buffer[256] = " EchoMacro1()";
			run_first_pass(buffer);

			std::string result = read_contents("EchoMacro1.txt");

			Assert::AreEqual("Hello World", result.c_str());
		};

		TEST_METHOD(EchoMacro2)
		{
			char buffer[256] = " EchoMacro2(\"Test string 2\")";
			run_first_pass(buffer);

			std::string result = read_contents("EchoMacro2.txt");
			Assert::AreEqual("Test string 2", result.c_str());
		};

		TEST_METHOD(EchoMacro3)
		{
			char buffer[256] = " EchoMacro3(11, \"Test string 3\")";
			run_first_pass(buffer);

			std::string result = read_contents("EchoMacro3.txt");

			Assert::AreEqual("My prefix (89): Test string 3", result.c_str());
		};

		TEST_METHOD(EchoMacro4)
		{
			char buffer[256] = " .echomacro4 11, \"Test string 4\"";
			error_occurred = false;
			run_first_pass(buffer);

			Assert::IsFalse(error_occurred);

			std::string result = read_contents("EchoMacro4.txt");

			Assert::AreEqual("My prefix (11): Test string 4", result.c_str());
		};

		//TEST_METHOD(EmitDefine1)
		//{
		//	char buffer[256] = " .db EmitDefine1";
		//	int session = StartSPASMErrorSession();
		//	run_first_pass(buffer);
		//	Assert::AreEqual(false, IsSPASMErrorSessionFatal(session), "Errors occurred");
		//	ReplaySPASMErrorSession(session);
		//	EndSPASMErrorSession(session);

		//	Assert::AreEqual((BYTE)10, output_contents[0], "Output value was wrong");
		//};

		//TEST_METHOD(EmitDefine2)
		//{
		//	char buffer[256] = " .db EmitDefine2";
		//	int session = StartSPASMErrorSession();
		//	run_first_pass(buffer);
		//	Assert::AreEqual(false, IsSPASMErrorSessionFatal(session), "Errors occurred");
		//	ReplaySPASMErrorSession(session);
		//	EndSPASMErrorSession(session);

		//	Assert::AreEqual((BYTE)33, (BYTE)(output_contents[0] + output_contents[1] + output_contents[2]), "Output value was wrong");
		//};

		//TEST_METHOD(EmitDefine3)
		//{
		//	char buffer[256] = " .db EmitDefine3";
		//	int session = StartSPASMErrorSession();
		//	run_first_pass(buffer);
		//	Assert::AreEqual(false, IsSPASMErrorSessionFatal(session), "Errors occurred");
		//	ReplaySPASMErrorSession(session);
		//	EndSPASMErrorSession(session);

		//	Assert::AreEqual((BYTE)33, (BYTE)(output_contents[0] + output_contents[1] + output_contents[2]), "Output value was wrong");
		//};

		//// This test verifies that errors that occur parsing element get passed up
		//TEST_METHOD(EmitDefine4)
		//{
		//	char buffer[256] = " .db EmitDefine4";
		//	int session = StartSPASMErrorSession();
		//	run_first_pass(buffer);
		//	Assert::AreEqual((DWORD)SPASM_ERR_VALUE_EXPECTED, GetLastSPASMError(), "Error did not occur when it should have");
		//	ReplaySPASMErrorSession(session);
		//	EndSPASMErrorSession(session);
		//};

		//// This test verifies that string defines are correctly parsed
		//TEST_METHOD(EmitDefine5)
		//{
		//	char buffer[256] = " .db EmitDefine5";
		//	int session = StartSPASMErrorSession();
		//	run_first_pass(buffer);
		//	ReplaySPASMErrorSession(session);
		//	Assert::AreEqual(false, IsSPASMErrorSessionFatal(session), "Errors occurred");
		//	EndSPASMErrorSession(session);

		//	Assert::AreEqual((BYTE) 'A', (BYTE)output_contents[0], "Output value was wrong");
		//};

		//TEST_METHOD(ConcatMacro1)
		//{
		//	char buffer[256] = " ConcatMacro1(12)";
		//	error_occurred = false;
		//	run_first_pass(buffer);

		//	Assert::IsFalse(error_occurred);

		//	System::IO::StreamReader ^sr = gcnew System::IO::StreamReader("ConcatMacro1.txt");
		//	System::String ^str = sr->ReadLine();

		//	Assert::AreEqual(gcnew String("$C"), str);
		//};

		//TEST_METHOD(ConcatMacro2)
		//{
		//	char buffer[256] = " ConcatMacro2(test)";
		//	error_occurred = false;
		//	run_first_pass(buffer);

		//	Assert::IsFalse(error_occurred);

		//	System::IO::StreamReader ^sr = gcnew System::IO::StreamReader("ConcatMacro2.txt");
		//	System::String ^str = sr->ReadLine();

		//	Assert::AreEqual(gcnew String("test"), str);
		//};

		//TEST_METHOD(ConcatMacro3)
		//{
		//	char buffer[256] = " ConcatMacro3(6)";
		//	error_occurred = false;
		//	run_first_pass(buffer);

		//	Assert::IsFalse(error_occurred);

		//	System::IO::StreamReader ^sr = gcnew System::IO::StreamReader("ConcatMacro3.txt");
		//	System::String ^str = sr->ReadLine();

		//	Assert::AreEqual(gcnew String("XXXXXX"), str);
		//};

		//TEST_METHOD(ConcatMacro4)
		//{
		//	char buffer[256] = " ConcatMacro4(8)";
		//	error_occurred = false;
		//	run_first_pass(buffer);
		//	Assert::IsFalse(error_occurred);

		//	System::IO::StreamReader ^sr = gcnew System::IO::StreamReader("ConcatMacro4.txt");
		//	System::String ^str = sr->ReadLine();

		//	Assert::AreEqual(gcnew String("XXXXXXXX"), str);
		//};

		//TEST_METHOD(ConcatMacro5)
		//{
		//	char buffer[256] = " ConcatMacro5()";
		//	error_occurred = false;
		//	run_first_pass(buffer);
		//	Assert::IsFalse(error_occurred);

		//	System::IO::StreamReader ^sr = gcnew System::IO::StreamReader("ConcatMacro5.txt");
		//	System::String ^str = sr->ReadLine();

		//	Assert::AreEqual(gcnew String("XXXXXXX"), str);
		//};

		//void IsWithinRange(int v1, int v2)
		//{
		//	int diff = abs(v1 - v2);
		//	char str[256];
		//	sprintf_s(str, "Value of off by %d", v2 - v1);
		//	Assert::IsTrue(diff < 15, gcnew String(str));
		//}

		//TEST_METHOD(Fixed14Power2)
		//{
		//	bool success;
		//	int result;
		//	success = parse_num("Fixed14Power(25735, 2)", &result);
		//	Assert::IsTrue(success);
		//	IsWithinRange(40425, result);
		//};

		//TEST_METHOD(Fixed14Power3)
		//{
		//	bool success;
		//	int result;
		//	success = parse_num("Fixed14Power(25735, 3)", &result);
		//	Assert::IsTrue(success);
		//	IsWithinRange(63500, result);
		//};

		//TEST_METHOD(Fixed14Power4)
		//{
		//	bool success;
		//	int result;
		//	success = parse_num("Fixed14Power(25735, 4)", &result);
		//	Assert::IsTrue(success);
		//	IsWithinRange(99732, result);
		//};

		//TEST_METHOD(Fixed14Cos15)
		//{
		//	bool success;
		//	int result;
		//	success = parse_num("Fixed14Cos(15)", &result);
		//	Assert::IsTrue(success);
		//	IsWithinRange(System::Math::Cos(15.0 * System::Math::PI / 180.0) * 16384.0, result);
		//};

		//TEST_METHOD(Fixed14Cos30)
		//{
		//	bool success;
		//	int result;
		//	success = parse_num("Fixed14Cos(30)", &result);
		//	Assert::IsTrue(success);
		//	IsWithinRange(System::Math::Cos(30.0 * System::Math::PI / 180.0) * 16384.0, result);
		//};

		//TEST_METHOD(Fixed14Cos45)
		//{
		//	bool success;
		//	int result;
		//	success = parse_num("Fixed14Cos(45)", &result);
		//	Assert::IsTrue(success);
		//	IsWithinRange(System::Math::Cos(45.0 * System::Math::PI / 180.0) * 16384.0, result);
		//};

		//TEST_METHOD(Fixed14Cos60)
		//{
		//	bool success;
		//	int result;
		//	success = parse_num("Fixed14Cos(60)", &result);
		//	Assert::IsTrue(success);
		//	IsWithinRange(System::Math::Cos(60.0 * System::Math::PI / 180.0) * 16384.0, result);
		//};

		//TEST_METHOD(Fixed14Cos75)
		//{
		//	bool success;
		//	int result;
		//	success = parse_num("Fixed14Cos(75)", &result);
		//	Assert::IsTrue(success);
		//	IsWithinRange(System::Math::Cos(75.0 * System::Math::PI / 180.0) * 16384.0, result);
		//};

		//TEST_METHOD(Fixed14Cos90)
		//{
		//	bool success;
		//	int result;
		//	success = parse_num("Fixed14Cos(90)", &result);
		//	Assert::IsTrue(success);

		//	IsWithinRange(System::Math::Cos(90.0 * System::Math::PI / 180.0) * 16384.0, result);
		//};
	};
}