#include "pch.h"

#include "tester.h"
#include "database.h"

#define BUFFSIZE 4096

static inline ull GetTime()
{
    return std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch()
    ).count();
}

Tester::Tester(Database* _db)
    : db(_db)
{
	// Start threadpool?
}

Tester::~Tester()
{
	// Stop threadpool?
}

void Tester::RunTest(const std::string& task, const std::string& name) const
{
	// Spawn thread to run tests on
	// Allows HTTP server to keep running while the test is running
	std::thread t1(&Tester::StartTest, this, task, name);
	t1.detach();
}

// ---------- PRIVATE ----------

void Tester::StartTest(const std::string task, const std::string name) const
{
	const std::string path = "C:\\dev\\CodeCritic\\CodeCritic\\website\\opgaver\\" + task + "\\";
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
    const ull timeLimit = 1000;
    const int testCount = 3;
    const std::string testData[testCount] = {
        "1 2", "2 5 4", "10 1 2 3 4 5 6 7 8 9 69"
    };
    //
    //

	// Run test cases
    int points = 0;
    for (uint c = 0; c < testCount; c++)
    {
        int point = Test(judgePath, testPath, testData[c], timeLimit);
        if (point > 0)
        {
            points += point;
        }
        else if (point == 0)
        {
            // Wrong answer
        }
        else if (point == -1337)
        {
            // Timelimit exceeded
        }
        else
        {
            // Error
            std::cout << "Error during testing\n";
        }
    }
    std::cout << "Score: " << points << '/' << testCount << "\n";

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

int Tester::Test(const LPCWSTR& judgePath, const LPCWSTR& testPath, const std::string& testData, const ull timeLimit) const
{
    // https://learn.microsoft.com/en-us/windows/win32/procthread/creating-a-child-process-with-redirected-input-and-output?redirectedfrom=MSDN

    // Prepare test
    int points = 0;
    std::vector<std::string> inputs;
    std::vector<std::string> outputs;
    
    // Create pipes for std I/O for judge
    HANDLE judgeStdInRead = NULL;
    HANDLE judgeStdInWrite = NULL;
    HANDLE judgeStdOutRead = NULL;
    HANDLE judgeStdOutWrite = NULL;

    // Create pipes for std I/O for submission
    HANDLE childStdInRead = NULL;
    HANDLE childStdInWrite = NULL;
    HANDLE childStdOutRead = NULL;
    HANDLE childStdOutWrite = NULL;

    SECURITY_ATTRIBUTES security;
    security.nLength = sizeof(SECURITY_ATTRIBUTES);
    security.bInheritHandle = TRUE;
    security.lpSecurityDescriptor = NULL;

    // Create pipe for the judge stdout
    if (!CreatePipe(&judgeStdOutRead, &judgeStdOutWrite, &security, 0))
    {
        return -1;
    }
    // read handle to stdout should not be inherited
    if (!SetHandleInformation(judgeStdOutRead, HANDLE_FLAG_INHERIT, 0))
    {
        return -2;
    }

    // Create pipe for the judge stdin
    if (!CreatePipe(&judgeStdInRead, &judgeStdInWrite, &security, 0))
    {
        return -3;
    }
    // read handle to stdin should not be inherited
    if (!SetHandleInformation(judgeStdInWrite, HANDLE_FLAG_INHERIT, 0))
    {
        return -4;
    }


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
    



    // Specify stdin and stdout handles for judge
    STARTUPINFO startInfoJudge;
    ZeroMemory(&startInfoJudge, sizeof(STARTUPINFO));
    startInfoJudge.cb = sizeof(STARTUPINFO);
    startInfoJudge.hStdError = judgeStdOutWrite;
    startInfoJudge.hStdOutput = judgeStdOutWrite;
    startInfoJudge.hStdInput = judgeStdInRead;
    startInfoJudge.dwFlags |= STARTF_USESTDHANDLES;

    PROCESS_INFORMATION processInfoJudge;
    ZeroMemory(&processInfoJudge, sizeof(PROCESS_INFORMATION));

    // Start the judge
    const BOOL judgeSuccess = CreateProcess(
        judgePath,      // Path to binary
        NULL,           // Command line
        NULL,           // Process handle not inheritable
        NULL,           // Thread handle not inheritable
        TRUE,           // Set handle inheritance
        0,              // No creation flags
        NULL,           // Use parent's environment block
        NULL,           // Use parent's starting directory 
        &startInfoJudge,     // Pointer to STARTUPINFO
        &processInfoJudge    // Pointer to PROCESS_INFORMATION
    );
    if (!judgeSuccess)
    {
        std::cout << "CreateProcess failed for judge: " << GetLastError() << "\n";
        return -5;
    }
    
    WriteToPipe(judgeStdInWrite, testData);
    while (outputs.size() < 1 || outputs[outputs.size() - 1].find('\r') == std::string::npos)
    {
        ReadFromPipe(judgeStdOutRead, outputs, true);
    }

    // Specify stdin and stdout handles for submission
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
        testPath,       // Path to binary
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
        std::cout << "CreateProcess failed for submission: " << GetLastError() << "\n";
        return -6;
    }

    // Close no longer needed handles
    CloseHandle(judgeStdOutWrite);
    CloseHandle(judgeStdInRead);
    CloseHandle(childStdOutWrite);
    CloseHandle(childStdInRead);

    // Start timer
    const ull start = GetTime();
    ull now = start;

    // Start test by starting to write input
    WriteToPipe(childStdInWrite, testData);

    while (now - start <= timeLimit)
    {
        if (ReadFromPipe(childStdOutRead, inputs))
        {
            WriteToPipe(judgeStdInWrite, inputs[inputs.size() - 1]);
            std::cout << "child stdout: '" << inputs[inputs.size() - 1] << "'\n";
        }
        if (ReadFromPipe(judgeStdOutRead, outputs))
        {
            const std::string& output = outputs[outputs.size() - 1];
            const size_t pos = output.find('\r');
            if (pos != std::string::npos)
            {
                std::cout << "judge answer: '" << output.substr(pos + 1) << "'\n";
                points = std::stoi(output.substr(pos + 1));
                break;
            }
            else
            {
                WriteToPipe(childStdInWrite, output);
                std::cout << "judge interaction: '" << output << "'\n";
            }
        }

        now = GetTime();
    }
    if (now - start > timeLimit)
    {
        std::cout << "TIMELIMIT EXCEEDED!\n";
        points = -1337;
    }

    
    // Close all other handles
    CloseHandle(judgeStdInWrite);
    CloseHandle(judgeStdOutRead);
    CloseHandle(childStdInWrite);
    CloseHandle(childStdOutRead);

    // Wait until child processes exit.
    WaitForSingleObject(processInfoJudge.hProcess, 1000);
    WaitForSingleObject(processInfo.hProcess, 1000);

    // Close the process handles
    CloseHandle(processInfoJudge.hProcess);
    CloseHandle(processInfoJudge.hThread);
    CloseHandle(processInfo.hProcess);
    CloseHandle(processInfo.hThread);

	// Return result of test
	return points;
}

void Tester::WriteToPipe(const HANDLE& pipeHandle, const std::string& msg) const
{
	// Allocate memory for msg size + 1 (the +1 is for a break character at the end '\n')
	const size_t size = 1 + msg.size();
    CHAR* inputBuf = new CHAR[size];
	if (inputBuf == nullptr)
	{
		return;
	}

    // Transfer the entire msg into the char* buffer
	for (uint c = 0; c < size - 1; c++)
	{
		inputBuf[c] = msg[c];
	}
	inputBuf[size - 1] = '\n';

    // Write to the handle with from buffer
	DWORD dwWrite;
	WriteFile(pipeHandle, inputBuf, size, &dwWrite, NULL);

	// Cleanup
	delete[] inputBuf;
}

bool Tester::ReadFromPipe(const HANDLE& pipeHandle, std::vector<std::string>& vec, const bool waitForData) const
{
    DWORD available;
    if (!waitForData)
    {
        // Check if there is anything to be read
        const BOOL success = PeekNamedPipe(pipeHandle, NULL, 0, NULL, &available, NULL);
        if (!(success && available > 0))
        {
            return false;
        }
    }
    else
    {
        // Set available amount of bytes to 1 when waitForData is true
        // This will cause this function to wait until at least one byte has been read
        available = 1;
    }

    // Create space in vector for new input data
    const size_t c = vec.size();
    vec.emplace_back("");

    // Create buffer for reading all the available bytes one chunk at a time (up to BUFFSIZE at a time)
	DWORD dwRead;
	CHAR outputBuf[BUFFSIZE];
	while (available > 0)
	{
        // Read into buffer, if reading fails, return false
        const BOOL read = ReadFile(pipeHandle, outputBuf, BUFFSIZE - 1, &dwRead, NULL);
        if (!read)
        {
            return false;
        }
        
        // Update amount of available bytes
        available -= BUFFSIZE - 1 > available ? available : BUFFSIZE - 1;

        // Save buffer
		outputBuf[dwRead] = 0;
		vec[c] += outputBuf;
	}
    return true;
}
