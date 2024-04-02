#include "pch.h"

#include "tester.h"

#define BUFFSIZE 4096

Tester::Tester()
{
	// Start threadpool?
}

Tester::~Tester()
{
	// Stop threadpool?
}

void Tester::RunTest(const std::string name) const
{
	// Spawn thread to run tests on
	// Allows HTTP server to keep running while the test is running
	std::thread t1(&Tester::StartTest, this, name);
	t1.detach();
}

// ---------- PRIVATE ----------

void Tester::StartTest(const std::string name) const
{
	const std::string path = "C:\\dev\\CodeCritic\\Opgaver\\" + name + "\\";
    const std::string sJudgePath = path + "judge.exe";

	// Compile submission
	const std::string compilePath = Compile(path + name + ".cpp");

    // Convert file paths to LPCWSTR
    const std::wstring wTestPathName = std::wstring(compilePath.begin(), compilePath.end());
    const std::wstring wJudgePathName = std::wstring(sJudgePath.begin(), sJudgePath.end());
    const LPCWSTR testPath = wTestPathName.c_str();
    const LPCWSTR judgePath = wJudgePathName.c_str();

    // Read config
    //
    //
    //

	// Run test cases
    int points = 0;
    for (uint c = 0; c < 10; c++)
    {
    	points += Test(judgePath, testPath, 1);
    }

	// Save test result in DB
	SaveScore(points);

	// Remove compiled file after all tests
	Delete(compilePath);
}

std::string Tester::Compile(const std::string& path) const
{
	// change filepath .cpp to .exe for output file
	const std::string compilePath = path.substr(0, path.size() - 4) + ".exe";

	// Compile file on given path
	const std::string cmd = "g++ --std=c++17 -O2 -o " + compilePath + " " + path;
	std::system(cmd.c_str());

	return compilePath;
}

void Tester::SaveScore(const int score) const
{
	// Save score to DB
}

void Tester::Delete(const std::string& deletePath) const
{
	// Delete file
	std::remove(deletePath.c_str());
}

int Tester::Test(const LPCWSTR& judgePath, const LPCWSTR& testPath, const int timeLimit) const
{
    // https://learn.microsoft.com/en-us/windows/win32/procthread/creating-a-child-process-with-redirected-input-and-output?redirectedfrom=MSDN

    // Prepare test
    //std::vector<std::string> inputs;
    //std::vector<std::string> outputs;
    // int (*Test)(std::vector<std::string>&inputs,
    // std::vector<std::string>&outputs)
    
    // Create pipes for std I/O
    HANDLE childStdInRead = NULL;
    HANDLE childStdInWrite = NULL;
    HANDLE childStdOutRead = NULL;
    HANDLE childStdOutWrite = NULL;

    SECURITY_ATTRIBUTES security;
    security.nLength = sizeof(SECURITY_ATTRIBUTES);
    security.bInheritHandle = TRUE;
    security.lpSecurityDescriptor = NULL;

    // Create pipe for the child stdout
    if (!CreatePipe(&childStdOutRead, &childStdOutWrite, &security, 0))
    {
        return -1;
    }
    // read handle to stdout should not be inherited
    if (!SetHandleInformation(childStdOutRead, HANDLE_FLAG_INHERIT, 0))
    {
        return -2;
    }

    // Create pipe for the child stdin
    if (!CreatePipe(&childStdInRead, &childStdInWrite, &security, 0))
    {
        return -3;
    }
    // read handle to stdin should not be inherited
    if (!SetHandleInformation(childStdInWrite, HANDLE_FLAG_INHERIT, 0))
    {
        return -4;
    }

    // Specify stdin and stdout handles
    STARTUPINFO startInfo;
    ZeroMemory(&startInfo, sizeof(STARTUPINFO));
    startInfo.cb = sizeof(STARTUPINFO);
    startInfo.hStdError = childStdOutWrite;
    startInfo.hStdOutput = childStdOutWrite;
    startInfo.hStdInput = childStdInRead;
    startInfo.dwFlags |= STARTF_USESTDHANDLES;

    PROCESS_INFORMATION processInfo;
    ZeroMemory(&processInfo, sizeof(PROCESS_INFORMATION));

    //// Start the script as a child process
    //// Microsoft documentation:
    //// https://learn.microsoft.com/en-us/windows/win32/procthread/creating-processes
    const BOOL success = CreateProcess(
        testPath,           // Path to binary
        NULL,           // Command line
        NULL,           // Process handle not inheritable
        NULL,           // Thread handle not inheritable
        TRUE,           // Set handle inheritance
        0,              // No creation flags
        NULL,           // Use parent's environment block
        NULL,           // Use parent's starting directory 
        &startInfo,     // Pointer to STARTUPINFO
        &processInfo    // Pointer to PROCESS_INFORMATION
    );

    if (!success)
    {
        return -5;
    }

    // Close no longer needed handles
    CloseHandle(childStdOutWrite);
    CloseHandle(childStdInRead);

    //int outputCode = Test(inputs, outputs);






    //for (unsigned int c = 0; outputCode == -1; c++)
    //{
    //    // Write to scripts stdin
    //    if (!WriteToPipe(outputs[c], inWrite))
    //    {
    //        //res.status = -6;
    //        //return res;
    //        return -6;
    //    }

    //    // Reads stdout from script into the end of inputs vector
    //    ReadFromPipe(outRead, inputs);

    //    outputCode = Test(inputs, outputs);
    //}
    ////res.result = outputCode;

    //// Wait until child process exits.
    //WaitForSingleObject(processInfo.hProcess, timeLimit);

    //// Close all other handles
    CloseHandle(processInfo.hProcess);
    CloseHandle(processInfo.hThread);
    CloseHandle(childStdInWrite);
    CloseHandle(childStdOutRead);

	// Return result of test
	return 0;
}

bool Tester::WriteToPipe(const std::string& msg, const HANDLE& pipeHandle)
{
	// Allocate memory for msg size + 1 (the +1 is for a break character at the end '\n')
	const size_t size = (1 + msg.size()) * sizeof(CHAR);

	CHAR* inputBuf = (CHAR*)malloc(size);
	if (inputBuf == nullptr)
	{
		return false;
	}

	for (unsigned int i = 0; i < size - 1; i++)
	{
		inputBuf[i] = msg[i];
	}
	inputBuf[msg.size()] = '\n';

	DWORD dwWrite;
	WriteFile(pipeHandle, inputBuf, size, &dwWrite, NULL);

	// Cleanup
	free(inputBuf);
	return true;
}

void Tester::ReadFromPipe(const HANDLE& pipeHandle, std::vector<std::string>& vec)
{
	DWORD dwRead;
	std::string output = "";

	CHAR outputBuf[BUFFSIZE];
	while (ReadFile(pipeHandle, outputBuf, BUFFSIZE - 1, &dwRead, NULL))
	{
		outputBuf[dwRead] = 0;
		output += outputBuf;

		// Check for break
		if (output[output.size() - 1] == '\n')
		{
			output.erase(output.begin() + output.size() - 2, output.end());
			break;
		}
	}

	vec.push_back(output);
}
