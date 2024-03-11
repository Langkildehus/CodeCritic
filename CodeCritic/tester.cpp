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

void Tester::RunTest(const std::string& name) const
{
	// Spawn thread to run tests on
	// Allows HTTP server to keep running while the test is running
	std::thread t1(&Tester::StartTest, this, name);
	t1.detach();
}

// ---------- PRIVATE ----------

void Tester::StartTest(const std::string& name) const
{
	std::string path = "C:\\dev\\CodeCritic\\Opgaver\\" + name + "\\" + name + ".cpp";

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
	//std::system(cmd.c_str());

	return compilePath;
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

int Tester::Test(const std::string& compiledPath) const
{
	// Run test on file


    // int timeLimit,
    // int (*Test)(std::vector<std::string>&inputs,
    // std::vector<std::string>&outputs)
    
    // Convert filepath to LPCWSTR
    //std::wstring wPathName = std::wstring(compiledPath.begin(), compiledPath.end());
    //LPCWSTR path = wPathName.c_str();

    //std::vector<std::string> inputs;
    //std::vector<std::string> outputs;

    //// Create pipes for std I/O
    //HANDLE inRead = 0;
    //HANDLE inWrite = 0;
    //HANDLE outRead = 0;
    //HANDLE outWrite = 0;

    //SECURITY_ATTRIBUTES security;
    //security.nLength = sizeof(SECURITY_ATTRIBUTES);
    //security.bInheritHandle = NULL;
    //security.bInheritHandle = TRUE;
    //security.lpSecurityDescriptor = NULL;

    //if (!CreatePipe(&outRead, &outWrite, &security, 0))
    //{
    //    //res.msg = "Error: StdoutRead CreatePipe";
    //    //res.status = -1;
    //    //return res;
    //    return -1;
    //}
    //if (!SetHandleInformation(outRead, HANDLE_FLAG_INHERIT, 0))
    //{
    //    //res.msg = "Stdout SetHandleInformation";
    //    //res.status = -2;
    //    //return res;
    //    return -2;
    //}
    //if (!CreatePipe(&inRead, &inWrite, &security, 0))
    //{
    //    //res.msg = "Stdin CreatePipe";
    //    //res.status = -3;
    //    //return res;
    //    return -3;
    //}
    //if (!SetHandleInformation(inWrite, HANDLE_FLAG_INHERIT, 0))
    //{
    //    //res.msg = "Stdin SetHandleInformation";
    //    //res.status = -4;
    //    //return res;
    //    return -4;
    //}

    //// Prepare to start target script
    //STARTUPINFO startInfo;
    //ZeroMemory(&startInfo, sizeof(startInfo));
    //startInfo.cb = sizeof(startInfo);
    //startInfo.hStdError = outWrite;
    //startInfo.hStdOutput = outWrite;
    //startInfo.hStdInput = inRead;
    //startInfo.dwFlags |= STARTF_USESTDHANDLES;

    //PROCESS_INFORMATION processInfo;
    //ZeroMemory(&processInfo, sizeof(processInfo));

    //// Start the script as a child process
    //// Microsoft documentation:
    //// https://learn.microsoft.com/en-us/windows/win32/procthread/creating-processes
    //if (!CreateProcess(
    //    path,           // Path to binary
    //    NULL,           // Command line
    //    NULL,           // Process handle not inheritable
    //    NULL,           // Thread handle not inheritable
    //    TRUE,           // Set handle inheritance
    //    0,              // No creation flags
    //    NULL,           // Use parent's environment block
    //    NULL,           // Use parent's starting directory 
    //    &startInfo,     // Pointer to STARTUPINFO
    //    &processInfo    // Pointer to PROCESS_INFORMATION
    //))
    //{
    //    //res.msg = "CreateProcess failed " + GetLastError();
    //    //res.status = -5;
    //    //return res;
    //    return -5;
    //}

    //// Close handles
    //CloseHandle(outWrite);
    //CloseHandle(inRead);

    //
    //
    //
    //
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
    //CloseHandle(processInfo.hProcess);
    //CloseHandle(processInfo.hThread);
    //CloseHandle(inWrite);
    //CloseHandle(outRead);
    //return outputCode;
















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
