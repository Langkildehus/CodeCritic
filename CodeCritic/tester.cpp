#include "pch.h"

#include "tester.h"
#include "database.h"

#define BUFFSIZE 4096

extern Database db;

static inline ull GetTime()
{
    return std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::high_resolution_clock::now().time_since_epoch()
    ).count();
}

Tester::Tester()
{
    // Start threadpool?
}

Tester::~Tester()
{
    // Stop threadpool?
}

void Tester::RunTest(const Cookie& cookies) const
{
    // Spawn thread to run tests on
    // Allows HTTP server to keep running while the test is running
    std::thread t1(&Tester::StartTest, this, cookies.assignment, cookies.username);
    t1.detach();
}

// ---------- PRIVATE ----------

void Tester::StartTest(const std::string assignment, const std::string username) const
{
    const std::string path = "C:\\dev\\CodeCritic\\CodeCritic\\website\\opgaver\\" + assignment + "\\";
    const std::string sJudgePath = path + "judge.exe";

    // Compile submission
    const std::string compilePath = Compile(path + username + ".cpp");

    // Convert file paths to LPCWSTR
    const std::wstring wTestPathName = std::wstring(compilePath.begin(), compilePath.end());
    const std::wstring wJudgePathName = std::wstring(sJudgePath.begin(), sJudgePath.end());
    const LPCWSTR testPath = wTestPathName.c_str();
    const LPCWSTR judgePath = wJudgePathName.c_str();

    // Read test cases
    std::vector<std::string> testCases;
    std::string line;
    std::ifstream testFile(path + "testcases.txt");
    while (std::getline(testFile, line))
    {
        testCases.push_back(line);
    }
    testFile.close();

    // Read config
    int timeLimit = 1000;
    std::ifstream configFile(path + "config.txt");
    int lineNumber = 0;
    while (std::getline(testFile, line))
    {
        switch (lineNumber)
        {
        case 0:
            timeLimit = std::stoi(line);
            break;
        }
        lineNumber++;
    }
    configFile.close();

    // Run test cases
    int points = 0;
    ull time = 0;
    for (uint c = 0; c < testCases.size(); c++)
    {
        const Result res = Test(judgePath, testPath, testCases[c], timeLimit);
        if (res.status == 0)
        {
            if (res.points > 0)
            {
                time += res.time;
                points += res.points;
            }
        }
        else if (res.status == 1)
        {
            std::cout << "Error creating pipes for judge!\n";
        }
        else if (res.status == 2)
        {
            std::cout << "Error creating pipes for submission!\n";
        }
        else if (res.status == 3)
        {
            std::cout << "Error while starting judge\n";
        }
        else if (res.status == 4)
        {
            std::cout << "Error while starting submission\n";
        }
        else
        {
            std::cout << "Timelimit exceeded!\n";
        }
    }
    std::cout << "Score: " << points << '/' << testCases.size() << "\n";

    // Save test result in DB
    SaveScore(assignment, username, points, time);

    // Remove compiled file after all tests
    Delete(compilePath);
}

inline std::string Tester::Compile(const std::string& path) const
{
    // change filepath .cpp to .exe for output file
    const std::string compilePath = path.substr(0, path.size() - 4) + ".exe";

    // Compile file on given path
    const std::string cmd = "g++ --std=c++17 -O2 -o " + compilePath + " " + path;
    std::system(cmd.c_str());

    return compilePath;
}

inline void Tester::SaveScore(const std::string& assignment, const std::string& username,
    const int points, const ull time) const
{
    // Save score to DB
    db.insertData(assignment, username, points, time);
    db.selectData(assignment);
}

inline void Tester::Delete(const std::string& deletePath) const
{
    // Delete file
    std::remove(deletePath.c_str());
}

Result Tester::Test(const LPCWSTR& judgePath, const LPCWSTR& testPath,
    const std::string& testData, const int timeLimit) const
{
    // https://learn.microsoft.com/en-us/windows/win32/procthread/creating-a-child-process-with-redirected-input-and-output?redirectedfrom=MSDN

    // Prepare test
    Result res{};
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
        res.status = 1;
        return res;
    }
    // read handle to stdout should not be inherited
    if (!SetHandleInformation(judgeStdOutRead, HANDLE_FLAG_INHERIT, 0))
    {
        res.status = 1;
        return res;
    }

    // Create pipe for the judge stdin
    if (!CreatePipe(&judgeStdInRead, &judgeStdInWrite, &security, 0))
    {
        res.status = 1;
        return res;
    }
    // read handle to stdin should not be inherited
    if (!SetHandleInformation(judgeStdInWrite, HANDLE_FLAG_INHERIT, 0))
    {
        res.status = 1;
        return res;
    }


    // Create pipe for the child stdout
    if (!CreatePipe(&childStdOutRead, &childStdOutWrite, &security, 0))
    {
        res.status = 2;
        return res;
    }
    // read handle to stdout should not be inherited
    if (!SetHandleInformation(childStdOutRead, HANDLE_FLAG_INHERIT, 0))
    {
        res.status = 2;
        return res;
    }

    // Create pipe for the child stdin
    if (!CreatePipe(&childStdInRead, &childStdInWrite, &security, 0))
    {
        res.status = 2;
        return res;
    }
    // read handle to stdin should not be inherited
    if (!SetHandleInformation(childStdInWrite, HANDLE_FLAG_INHERIT, 0))
    {
        res.status = 2;
        return res;
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
        res.status = 3;
        return res;
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
        res.status = 4;
        return res;
    }

    {
        using namespace std::chrono_literals;
        std::this_thread::sleep_for(500ms);
    }

    ull start = GetTime();
    //std::string* writeStr = &testData;
    //std::thread judgeWriteThread(&Tester::WriteThread, this, judgeStdInWrite, writeStr);
    //std::thread submissionWriteThread(&Tester::WriteThread, this, childStdInWrite, writeStr);

    WriteToPipe(judgeStdInWrite, testData);
    while (GetTime() - start < 10 * timeLimit
        && (outputs.size() < 1 || outputs[outputs.size() - 1].find('\r') == std::string::npos))
    {
        ReadFromPipe(judgeStdOutRead, outputs);
    }

    // Start timer
    start = GetTime();
    ull now = start;

    // Start test by starting to write input
    WriteToPipe(childStdInWrite, testData);
    while (now - start <= timeLimit)
    {
        if (ReadFromPipe(childStdOutRead, inputs))
        {
            std::string& input = inputs[inputs.size() - 1];
            CleanFromENDL(input);

            std::cout << "Child stdout: '" << input << "'\n";
            WriteToPipe(judgeStdInWrite, input);
        }
        if (ReadFromPipe(judgeStdOutRead, outputs))
        {
            std::string& output = outputs[outputs.size() - 1];
            CleanFromENDL(output);
            const size_t pos = output.find('\r');

            if (pos != std::string::npos)
            {
                if (pos > 0)
                {
                    // Message pending before exit
                    const std::string lastInput = output.substr(0, pos);
                    WriteToPipe(childStdInWrite, lastInput);
                }

                WaitForSingleObject(processInfo.hProcess, timeLimit + start - now);
                now = GetTime();
                if (now - start > timeLimit)
                {
                    break;
                }

                std::cout << "Judge rating: " << output.substr(pos + 1) << '\n';
                res.points = std::stoi(output.substr(pos + 1));
                res.time = now - start;
                break;
            }
            else
            {
                std::cout << "Judge interaction: '" << output << "'\n";
                WriteToPipe(childStdInWrite, output);
            }
        }

        now = GetTime();
    }
    if (now - start > timeLimit)
    {
        res.status = 5;
    }

    // Close all handles
    DisconnectNamedPipe(judgeStdOutWrite);
    DisconnectNamedPipe(judgeStdInRead);
    DisconnectNamedPipe(childStdOutWrite);
    DisconnectNamedPipe(childStdInRead);
    CloseHandle(judgeStdOutWrite);
    CloseHandle(judgeStdInRead);
    CloseHandle(childStdOutWrite);
    CloseHandle(childStdInRead);

    DisconnectNamedPipe(judgeStdInWrite);
    DisconnectNamedPipe(judgeStdOutRead);
    DisconnectNamedPipe(childStdInWrite);
    DisconnectNamedPipe(childStdOutRead);
    CloseHandle(judgeStdInWrite);
    CloseHandle(judgeStdOutRead);
    CloseHandle(childStdInWrite);
    CloseHandle(childStdOutRead);

    // Start termination process (Process is async, so we need to wait afterwards)
    TerminateProcess(processInfoJudge.hProcess, 0);
    TerminateProcess(processInfo.hProcess, 0);

    // Wait until child processes have been terminated
    WaitForSingleObject(processInfoJudge.hProcess, INFINITE);
    WaitForSingleObject(processInfo.hProcess, INFINITE);

    // Close the process handles
    CloseHandle(processInfoJudge.hProcess);
    CloseHandle(processInfoJudge.hThread);
    CloseHandle(processInfo.hProcess);
    CloseHandle(processInfo.hThread);

    // Return result of test
    return res;
}

void Tester::WriteToPipe(const HANDLE& pipeHandle, const std::string& msg) const
{
    WriteFile(pipeHandle, msg.c_str(), msg.size(), NULL, NULL);
    WriteFile(pipeHandle, "\n", 1, NULL, NULL);
}

bool Tester::ReadFromPipe(const HANDLE& pipeHandle, std::vector<std::string>& vec, const bool waitForData) const
{
    DWORD available, dwRead;
    if (!waitForData)
    {
        // Check if there is anything to be read
        const BOOL success = PeekNamedPipe(pipeHandle, NULL, NULL, NULL, &available, NULL);
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
    if (!waitForData)
    {
        vec[c].reserve(available);
    }

    // Create buffer for reading all the available bytes one chunk at a time (up to BUFFSIZE at a time)
    CHAR outputBuf[BUFFSIZE];
    while (available > 0)
    {
        // Read into buffer, if reading fails, return false
        const BOOL read = ReadFile(pipeHandle, outputBuf, BUFFSIZE - 1 > available ? available : BUFFSIZE - 1, &dwRead, NULL);
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

/*void Tester::WriteThread(const HANDLE& pipeHandle, std::string* msg)
{
    while (true)
    {
        std::unique_lock<std::mutex> lck(writeMtx);
        while (!msg->length())
        {
            writeCnd.wait(lck);
        }
        WriteToPipe(pipeHandle, *msg);
    }
}*/

inline void Tester::CleanFromENDL(std::string& str) const
{
    size_t pos = str.find('\r');
    while (pos != std::string::npos)
    {
        if (str[pos + 1] == '\n')
        {
            str.erase(pos, 1);
            pos--;
        }
        pos = str.find('\r', pos + 1);
    }
}
