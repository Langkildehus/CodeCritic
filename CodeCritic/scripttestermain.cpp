#include "pch.h"

//#include "scripttester.h"
//
//const std::string file = "C:\\dev\\CodeCritic\\x64\\Debug\\Testscript.exe";
//
//int __main()
//{
//	const size_t len = 8 * sizeof(CHAR);
//	CHAR* testData = (CHAR*)malloc(len);
//
//	if (testData == nullptr)
//	{
//		std::cout << "Failed to allocate memory for testdata\n";
//		return -1;
//	}
//
//	testData[0] = '7';
//	testData[1] = 'a';
//	testData[2] = 'b';
//	testData[3] = 'c';
//	testData[4] = 'd';
//	testData[5] = 'e';
//	testData[6] = 'f';
//	testData[7] = 'g';
//
//	Result res = RunTest(file, testData, len);
//	if (res.status < 0)
//	{
//		std::cout << "Error while testing script:\n";
//	}
//	std::cout << res.msg << "\n";
//
//	return 0;
//}
//
//int Guess(std::vector<std::string>& inputs, std::vector<std::string>& outputs)
//{
//	if (inputs.size() == 0)
//	{
//		outputs.push_back("1000");
//		return -1;
//	}
//
//	std::string& strInput = inputs[inputs.size() - 1];
//	int input = std::stoi(strInput);
//
//	if (input == 333)
//	{
//		outputs.emplace_back("CORRECT");
//		return 1;
//	}
//	else if (input > 333)
//	{
//		outputs.emplace_back("LOWER");
//		return -1;
//	}
//	else
//	{
//		outputs.emplace_back("HIGHER");
//		return -1;
//	}
//}
//
//int main()
//{
//	Result res = RunInteractive(file, 1000, &Guess);
//	if (res.status < 0)
//	{
//		std::cout << "Error while testing script:\n";
//		std::cout << res.msg << "\n";
//		return -1;
//	}
//
//	std::cout << "Score: " << res.result << "\n";
//	return 0;
//}
