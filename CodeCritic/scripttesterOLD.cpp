#include "pch.h"

//#include "scripttester.h"
//
//#define BUFFSIZE 4096
//
//bool WriteToPipe(const std::string& msg, const HANDLE& pipeHandle)
//{
//    // Allocate memory for msg size + 1 (the +1 is for a break character at the end '\n')
//    const size_t size = (1 + msg.size()) * sizeof(CHAR);
//
//    CHAR* inputBuf = (CHAR*)malloc(size);
//    if (inputBuf == nullptr)
//    {
//        return false; // Error -6
//    }
//
//    for (unsigned int i = 0; i < size - 1; i++)
//    {
//        inputBuf[i] = msg[i];
//    }
//    inputBuf[msg.size()] = '\n';
//
//    DWORD dwWrite;
//    WriteFile(pipeHandle, inputBuf, size, &dwWrite, NULL);
//
//    // Cleanup
//    free(inputBuf);
//    return true;
//}
//
//void ReadFromPipe(const HANDLE& pipeHandle, std::vector<std::string>& vec)
//{
//    DWORD dwRead;
//    std::string output = "";
//
//    CHAR outputBuf[BUFFSIZE + 1];
//    while (ReadFile(pipeHandle, outputBuf, BUFFSIZE, &dwRead, NULL))
//    {
//        outputBuf[dwRead] = 0;
//        output += outputBuf;
//
//        // Check for break
//        if (output[output.size() - 1] == '\n')
//        {
//            output.erase(output.begin() + output.size() - 2, output.end());
//            break;
//        }
//    }
//
//    vec.push_back(output);
//}
//
//Result RunTest(std::string file, CHAR* testData, size_t len)
//{
//    // Convert filepath to LPCWSTR
//    std::wstring wPathName = std::wstring(file.begin(), file.end());
//    LPCWSTR path = wPathName.c_str();
//
//    Result res;
//
//    // Create pipes for std i/o
//    HANDLE in_read = 0;
//    HANDLE in_write = 0;
//    HANDLE out_read = 0;
//    HANDLE out_write = 0;
//
//    SECURITY_ATTRIBUTES security;
//    security.nLength = sizeof(SECURITY_ATTRIBUTES);
//    security.bInheritHandle = NULL;
//    security.bInheritHandle = TRUE;
//    security.lpSecurityDescriptor = NULL;
//
//    if (!CreatePipe(&out_read, &out_write, &security, 0))
//    {
//        res.msg = "Error: StdoutRd CreatePipe";
//        res.status = -1;
//        return res;
//    }
//    if (!SetHandleInformation(out_read, HANDLE_FLAG_INHERIT, 0))
//    {
//        res.msg = "Stdout SetHandleInformation";
//        res.status = -2;
//        return res;
//    }
//    if (!CreatePipe(&in_read, &in_write, &security, 0))
//    {
//        res.msg = "Stdin CreatePipe";
//        res.status = -3;
//        return res;
//    }
//    if (!SetHandleInformation(in_write, HANDLE_FLAG_INHERIT, 0))
//    {
//        res.msg = "Stdin SetHandleInformation";
//        res.status = -4;
//        return res;
//    }
//
//    // Start target script
//    STARTUPINFO startInfo;
//    PROCESS_INFORMATION processInfo;
//
//    ZeroMemory(&startInfo, sizeof(startInfo));
//    startInfo.cb = sizeof(startInfo);
//    startInfo.hStdError = out_write;
//    startInfo.hStdOutput = out_write;
//    startInfo.hStdInput = in_read;
//    startInfo.dwFlags |= STARTF_USESTDHANDLES;
//
//    ZeroMemory(&processInfo, sizeof(processInfo));
//
//    // Write cin to program
//    DWORD dwWrite;
//
//    WriteFile(in_write, testData, len, &dwWrite, NULL);
//
//    // Start the child process
//    // Documentation:
//    // https://learn.microsoft.com/en-us/windows/win32/procthread/creating-processes
//    if (!CreateProcess(
//        path,           // Path to binary
//        NULL,           // Command line
//        NULL,           // Process handle not inheritable
//        NULL,           // Thread handle not inheritable
//        TRUE,           // Set handle inheritance
//        0,              // No creation flags
//        NULL,           // Use parent's environment block
//        NULL,           // Use parent's starting directory 
//        &startInfo,     // Pointer to STARTUPINFO
//        &processInfo    // Pointer to PROCESS_INFORMATION
//    ))
//    {
//        res.msg = "CreateProcess failed " + GetLastError();
//        res.status = -5;
//        return res;
//    }
//
//    // Wait until child process exits.
//    WaitForSingleObject(processInfo.hProcess, INFINITE);
//
//    // Close handles
//    CloseHandle(processInfo.hProcess);
//    CloseHandle(processInfo.hThread);
//    CloseHandle(out_write);
//    CloseHandle(in_read);
//
//    // Get ouput
//    DWORD dwRead;
//
//    CHAR chBuf[BUFFSIZE + 1];
//    while (ReadFile(out_read, chBuf, BUFFSIZE, &dwRead, NULL))
//    {
//        chBuf[dwRead] = 0;
//        res.msg += chBuf;
//    }
//
//    CloseHandle(in_write);
//    CloseHandle(out_read);
//
//    return res;
//}
//
//Result RunInteractive(
//    std::string file,
//    int timeLimit,
//    int (*Test)(std::vector<std::string>& inputs, std::vector<std::string>& outputs)
//)
//{
//    // Convert filepath to LPCWSTR
//    std::wstring wPathName = std::wstring(file.begin(), file.end());
//    LPCWSTR path = wPathName.c_str();
//
//    Result res;
//    std::vector<std::string> inputs;
//    std::vector<std::string> outputs;
//
//    // Create pipes for std i/o
//    HANDLE inRead = 0;
//    HANDLE inWrite = 0;
//    HANDLE outRead = 0;
//    HANDLE outWrite = 0;
//
//    SECURITY_ATTRIBUTES security;
//    security.nLength = sizeof(SECURITY_ATTRIBUTES);
//    security.bInheritHandle = NULL;
//    security.bInheritHandle = TRUE;
//    security.lpSecurityDescriptor = NULL;
//
//    if (!CreatePipe(&outRead, &outWrite, &security, 0))
//    {
//        res.msg = "Error: StdoutRd CreatePipe";
//        res.status = -1;
//        return res;
//    }
//    if (!SetHandleInformation(outRead, HANDLE_FLAG_INHERIT, 0))
//    {
//        res.msg = "Stdout SetHandleInformation";
//        res.status = -2;
//        return res;
//    }
//    if (!CreatePipe(&inRead, &inWrite, &security, 0))
//    {
//        res.msg = "Stdin CreatePipe";
//        res.status = -3;
//        return res;
//    }
//    if (!SetHandleInformation(inWrite, HANDLE_FLAG_INHERIT, 0))
//    {
//        res.msg = "Stdin SetHandleInformation";
//        res.status = -4;
//        return res;
//    }
//
//    // Prepare to start target script
//    STARTUPINFO startInfo;
//    ZeroMemory(&startInfo, sizeof(startInfo));
//    startInfo.cb = sizeof(startInfo);
//    startInfo.hStdError = outWrite;
//    startInfo.hStdOutput = outWrite;
//    startInfo.hStdInput = inRead;
//    startInfo.dwFlags |= STARTF_USESTDHANDLES;
//
//    PROCESS_INFORMATION processInfo;
//    ZeroMemory(&processInfo, sizeof(processInfo));
//
//    // Start the script as a child process
//    // Documentation:
//    // https://learn.microsoft.com/en-us/windows/win32/procthread/creating-processes
//    if (!CreateProcess(
//        path,           // Path to binary
//        NULL,           // Command line
//        NULL,           // Process handle not inheritable
//        NULL,           // Thread handle not inheritable
//        TRUE,           // Set handle inheritance
//        0,              // No creation flags
//        NULL,           // Use parent's environment block
//        NULL,           // Use parent's starting directory 
//        &startInfo,     // Pointer to STARTUPINFO
//        &processInfo    // Pointer to PROCESS_INFORMATION
//    ))
//    {
//        res.msg = "CreateProcess failed " + GetLastError();
//        res.status = -5;
//        return res;
//    }
//
//    // Close handles
//    CloseHandle(outWrite);
//    CloseHandle(inRead);
//
//    int outputCode = Test(inputs, outputs);
//    for (unsigned int c = 0; outputCode == -1; c++)
//    {
//        // Write to scripts stdin
//        if (!WriteToPipe(outputs[c], inWrite))
//        {
//            res.status = -6;
//            return res;
//        }
//
//        // Reads stdout from script into the end of inputs vector
//        ReadFromPipe(outRead, inputs);
//
//        outputCode = Test(inputs, outputs);
//    }
//    res.result = outputCode;
//
//    // Wait until child process exits.
//    WaitForSingleObject(processInfo.hProcess, timeLimit);
//
//    // Close all other handles
//    CloseHandle(processInfo.hProcess);
//    CloseHandle(processInfo.hThread);
//    CloseHandle(inWrite);
//    CloseHandle(outRead);
//
//    return res;
//}
