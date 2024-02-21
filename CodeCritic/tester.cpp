#include "pch.h"

#include "tester.h"

Tester::Tester()
{
	// Start threadpool?
}

Tester::~Tester()
{
	// Stop threadpool?
}

Tester::RunTest(const std::string& path) const
{
	// Add path to queue
}

Tester::StartTest(const std::string& path) const
{
	std::string compiledPath = Compile(path);
	Test(compiledPath);

	// Cleanup
	Delete(compiledPath);
}

std::string Tester::Compile(const std::string& path) const
{
	// Compile file on given path
	
	// Return path to compile output
	return "HERE";
}

int Test(const std::string& compiledPath) const
{
	// Run test on file
	
	// Return result of test
	return 0;
}

void Delete(const std::string& deletePath) const
{
	// Delete file
}
