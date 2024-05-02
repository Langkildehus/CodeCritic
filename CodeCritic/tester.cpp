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

Tester::Tester() { }

Tester::~Tester()
{
    using namespace std::chrono_literals; // Used for std::this_thread::sleep_for()
    
    int locks = 0;
    while (true)
    {
        // Check to see if mutex can be locked 3 times in a row
        // When it can, all tests have been completed and class is safe to destruct
        if (runMtx.try_lock())
        {
            runMtx.unlock();
            locks++;
            if (locks > 2)
            {
                return;
            }
            std::this_thread::sleep_for(10ms);
        }
        else
        {
            locks = 0;
        }
    }
}

void Tester::RunTest(const Cookie& cookies, const SOCKET connection)
{
    if (cookies.assignment.size() < 1 || cookies.username.size() < 1 || cookies.language.size() < 1)
    {
        std::cout << "Insufficient cookies to run test!\n";
        return;
    }

    // Spawn thread to run tests on
    // Allows HTTP server to keep running while the test is running
    std::thread t1(&Tester::StartTest, this, cookies.assignment, cookies.username, cookies.language, connection);
    t1.detach();
}

// ---------- PRIVATE ----------

void Tester::StartTest(const std::string assignment, const std::string username,
    const std::string language, const SOCKET connection)
{
    // Wait for mutex lock
    std::lock_guard<std::mutex> lockGuard(runMtx);
    
    const std::string path = "website\\opgaver\\" + assignment + "\\";
    const std::string sJudgePath = path + "judge.exe";
    std::string sourceFile = path + username;

    if (language == "C++")
    {
        sourceFile += ".cpp";
    }
    else if (language == "C#")
    {
        sourceFile += ".cs";
    }
    else
    {
        std::cout << "Not a supported language\n";
        return;
    }

    // Compile submission
    const std::string compilePath = Compile(sourceFile, language);

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
    int attempts = 2;
    std::cout << "Starting test for " << username << ":\n";
    for (uint c = 0; c < testCases.size(); c++)
    {
        const Result res = Test(judgePath, testPath, testCases[c], timeLimit);
        std::cout << '[' << c + 1 << '/' << testCases.size() << "]: ";
        switch (res.status)
        {
        case 0:
            attempts = 2;
            if (res.points > 0)
            {
                time += res.time;
                points += res.points;
            }
            std::cout << "Points: " << res.points << ", Time taken: " << res.time << "\n";
            break;
        case 1:
            std::cout << "Error creating pipes for judge!\n";
            break;
        case 2:
            std::cout << "Error creating pipes for submission!\n";
            break;
        case 3:
            std::cout << "Error while starting judge\n";
            break;
        case 4:
            std::cout << "Error while starting submission\n";
            break;
        case 5:
            std::cout << "Timelimit exceeded!\n";
            if (attempts > 0)
            {
                attempts--;
                c--;
            }
            break;
        case 6:
            std::cout << "Judge failed to load test data\n";
            break;
        default:
            std::cout << "Unknown error during test\n";
            break;
        }
    }
    std::cout << "Score: " << points << '/' << testCases.size() << ", Time taken: " << time << "\n";

    // Save test result in DB
    SaveScore(assignment, username, points, time);

    // Create response header and body
    const std::string body = "{\"points\": " + std::to_string(points)
        + ", \"maxpoints\": " + std::to_string(testCases.size())
        + ", \"time\": " + std::to_string(time) + '}';
    const std::string response = "HTTP/1.1 201 OK\nContent-Type: application/json\nContent - Length: "
        + std::to_string(body.size()) + "\r\n\r\n";

    // Send response header and body
    send(connection, response.c_str(), response.size(), 0);
    send(connection, body.c_str(), body.size(), 0);
    closesocket(connection);

    // Remove compiled file after all tests
    Delete(compilePath);
    Delete(sourceFile);
}

inline std::string Tester::Compile(const std::string& path, const std::string& language) const
{
    // change filepath .cpp to .exe for output file
    const std::string compilePath = path.substr(0, path.size() - 4) + ".exe";
    std::string cmd;

    // Compile file on given path
    if (language == "C++")
    {
        cmd = "g++ --std=c++17 -O3 -mavx2 -o " + compilePath + " " + path;
    }
    else if (language == "C#")
    {
        cmd = "g++ --std=c++17 -O3 -mavx2 -o " + compilePath + " " + path;
    }
    std::system(cmd.c_str());

    return compilePath;
}

inline void Tester::SaveScore(const std::string& assignment, const std::string& username,
    const int points, const ull time) const
{
    // Save score to DB
    db.insertData(assignment, username, points, time);
}

inline void Tester::Delete(const std::string& deletePath) const
{
    // Delete file
    std::remove(deletePath.c_str());
}

Result Tester::Test(const LPCWSTR& judgePath, const LPCWSTR& testPath,
    std::string& testData, const int timeLimit) const
{
    // https://learn.microsoft.com/en-us/windows/win32/procthread/creating-a-child-process-with-redirected-input-and-output?redirectedfrom=MSDN

    // Prepare test
    using namespace std::chrono_literals; // Used for std::this_thread::sleep_for()
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
        // Give time for CreateProcess to finish starting the process
        std::this_thread::sleep_for(10ms);
    }

    WriteThreadData judgeWriteData{ judgeStdInWrite };
    std::thread judgeWriteThread;
    {
        std::unique_lock<std::mutex> lock(judgeWriteData.mtx);
        judgeWriteThread = std::thread(&Tester::WriteThread, this, &judgeWriteData);
        judgeWriteData.cnd.wait(lock);
    }
    
    WriteThreadData submissionWriteData{ childStdInWrite };
    std::thread submissionWriteThread;
    {
        std::unique_lock<std::mutex> lock(submissionWriteData.mtx);
        submissionWriteThread = std::thread(&Tester::WriteThread, this, &submissionWriteData);
        submissionWriteData.cnd.wait(lock);
    }

    judgeWriteData.str = &testData;
    judgeWriteData.cnd.notify_one();
    ull now, start = GetTime();
    while (GetTime() - start < 5 * timeLimit
        && (outputs.size() < 1 || outputs[outputs.size() - 1].find('\r') == std::string::npos))
    {
        ReadFromPipe(judgeStdOutRead, outputs);
    }
    if (GetTime() - start <= 5 * timeLimit)
    {
        // Start test
        submissionWriteData.str = &testData;
        submissionWriteData.cnd.notify_one();
        start = GetTime();
        now = start;

        while (now - start <= timeLimit)
        {
            if (ReadFromPipe(childStdOutRead, inputs))
            {
                std::string& input = inputs[inputs.size() - 1];
                CleanFromENDL(input);

                while (!judgeWriteData.mtx.try_lock())
                {
                    now = GetTime();
                    if (now - start > timeLimit)
                    {
                        goto breakout;
                    }
                }
                judgeWriteData.str = &inputs[inputs.size() - 1];
                judgeWriteData.mtx.unlock();
                judgeWriteData.cnd.notify_one();
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
                        while (!submissionWriteData.mtx.try_lock())
                        {
                            now = GetTime();
                            if (now - start > timeLimit)
                            {
                                goto breakout;
                            }
                        }
                        submissionWriteData.mtx.unlock();
                        WriteToPipe(childStdInWrite, lastInput); // Last input doesn't need to be threaded
                    }

                    WaitForSingleObject(processInfo.hProcess, timeLimit);
                    now = GetTime();
                    if (now - start > timeLimit)
                    {
                        break;
                    }

                    std::string result = output.substr(pos + 1);
                    for (int c = 0; c < result.size(); c++)
                    {
                        if (result[c] == '\r' || result[c] == '\n')
                        {
                            result.erase(c, 1);
                        }
                    }

                    res.points = std::stoi(result);
                    res.time = now - start;
                    break;
                }
                else
                {
                    while (!submissionWriteData.mtx.try_lock())
                    {
                        now = GetTime();
                        if (now - start > timeLimit)
                        {
                            goto breakout;
                        }
                    }
                    submissionWriteData.str = &outputs[outputs.size() - 1];
                    submissionWriteData.mtx.unlock();
                    submissionWriteData.cnd.notify_one();
                }
            }

            now = GetTime();
        }

    breakout:
        if (now - start > timeLimit)
        {
            res.status = 5;
        }
    }
    else
    {
        res.status = 6;
    }

    // Tell threads to close
    judgeWriteData.go = false;
    judgeWriteData.cnd.notify_one();
    submissionWriteData.go = false;
    submissionWriteData.cnd.notify_one();

    // Start termination process (Process is async, so we need to wait afterwards)
    TerminateProcess(processInfoJudge.hProcess, 0);
    TerminateProcess(processInfo.hProcess, 0);

    // Close all handles
    CloseHandle(judgeStdOutWrite);
    CloseHandle(judgeStdInRead);
    CloseHandle(childStdOutWrite);
    CloseHandle(childStdInRead);

    CloseHandle(judgeStdInWrite);
    CloseHandle(judgeStdOutRead);
    CloseHandle(childStdInWrite);
    CloseHandle(childStdOutRead);

    // Wait until child processes have been terminated
    WaitForSingleObject(processInfoJudge.hProcess, INFINITE);
    WaitForSingleObject(processInfo.hProcess, INFINITE);

    // Close the process handles
    CloseHandle(processInfoJudge.hProcess);
    CloseHandle(processInfoJudge.hThread);
    CloseHandle(processInfo.hProcess);
    CloseHandle(processInfo.hThread);

    // Wait for threads to exit
    judgeWriteThread.join();
    submissionWriteThread.join();

    // Return result of test
    return res;
}

void Tester::WriteToPipe(const HANDLE& pipeHandle, const std::string& msg) const
{
    WriteFile(pipeHandle, msg.c_str(), (DWORD)msg.size(), NULL, NULL);
    WriteFile(pipeHandle, "\n", 1, NULL, NULL);
}

bool Tester::ReadFromPipe(const HANDLE& pipeHandle, std::vector<std::string>& vec) const
{
    DWORD available, dwRead;

    // Check if there is anything to be read
    const BOOL success = PeekNamedPipe(pipeHandle, NULL, NULL, NULL, &available, NULL);
    if (!(success && available > 0))
    {
        return false;
    }

    // Create space in vector for new input data
    const size_t c = vec.size();
    vec.emplace_back("");
    vec[c].reserve(available);

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

void Tester::WriteThread(WriteThreadData* threadData) const
{
    threadData->cnd.notify_one();
    while (threadData->go)
    {
        // Acquire lock & wait
        std::unique_lock<std::mutex> lock(threadData->mtx);
        threadData->cnd.wait(lock);

        // Make sure there is something to read
        if (threadData->str == nullptr)
        {
            continue;
        }

        // Make sure thread is still supposed to run
        if (!threadData->go)
        {
            break;
        }
        
        // Write data and reset pointer
        WriteToPipe(threadData->pipeHandle, *threadData->str);
        threadData->str = nullptr;
    }
}

inline void Tester::CleanFromENDL(std::string& str) const
{
    size_t pos = str.find('\r');
    while (pos != std::string::npos)
    {
        if (str.size() > pos + 1 && str[pos + 1] == '\n')
        {
            str[pos] = ' ';
        }
        pos = str.find('\r', pos + 1);
    }
}
