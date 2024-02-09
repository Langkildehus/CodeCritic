#include <iostream>
#include <string>

#include "scripttester.h"

int main()
{
	const std::string file = "C:\\dev\\CodeCritic\\x64\\Debug\\Testscript.exe";

	const size_t len = 8 * sizeof(CHAR);
	CHAR* testData = (CHAR*)malloc(len);

	if (testData == nullptr)
	{
		std::cout << "Failed to allocate memory for testdata\n";
		return -1;
	}

	testData[0] = '7';
	testData[1] = 'a';
	testData[2] = 'b';
	testData[3] = 'c';
	testData[4] = 'd';
	testData[5] = 'e';
	testData[6] = 'f';
	testData[7] = 'g';

	Result res = RunTest(file, testData, len);
	if (res.status < 0)
	{
		std::cout << "Error while testing script:\n";
	}
	std::cout << res.msg << "\n";

	return 0;
}
