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

void Tester::RunTest(const std::string& path) const
{
	// Spawn thread to run tests on
	// Allows HTTP server to keep running while the test is running
	std::thread t1(&Tester::StartTest, this, path);
	t1.detach();
}

// ---------- PRIVATE ----------

void Tester::StartTest(const std::string& path) const
{
	// Compile submission
	std::string compilePath = Compile(path);

	// Run test
	int result = Test(compilePath);

	// Save test result in DB
	SaveScore(result);

	// Remove compiled file after all tests
	Delete(compilePath);
}

std::string Tester::Compile(const std::string& path) const
{
	// change filepath .cpp to .exe for output file
	std::string compilePath = path.substr(0, path.size() - 4) + ".exe";

	// Compile file on given path
	const std::string cmd = "g++ --std=c++17 -O2 -o " + compilePath + " " + path;
	std::system(cmd.c_str());

	return compilePath;
}

int Tester::Test(const std::string& compiledPath) const
{
	// Run test on file
	
	// Return result of test
	return 0;
}

void Tester::SaveScore(int score) const
{
	// Save score to DB
}

void Tester::Delete(const std::string& deletePath) const
{
	// Delete file
	std::remove(deletePath.c_str());
}
