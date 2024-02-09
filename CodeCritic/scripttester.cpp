#include <iostream>

#include "scripttester.h"

#define BUFFSIZE 4096

Result RunTest(std::string file, CHAR* testData, size_t len)
{
    // Convert filepath to LPCWSTR
    std::wstring wPathName = std::wstring(file.begin(), file.end());
    LPCWSTR path = wPathName.c_str();

    Result res;

    // Create pipes for std i/o
    HANDLE in_read = 0;
    HANDLE in_write = 0;
    HANDLE out_read = 0;
    HANDLE out_write = 0;

    SECURITY_ATTRIBUTES security;
    security.nLength = sizeof(SECURITY_ATTRIBUTES);
    security.bInheritHandle = NULL;
    security.bInheritHandle = TRUE;
    security.lpSecurityDescriptor = NULL;

    if (!CreatePipe(&out_read, &out_write, &security, 0))
    {
        res.msg = "Error: StdoutRd CreatePipe";
        res.status = -1;
        return res;
    }
    if (!SetHandleInformation(out_read, HANDLE_FLAG_INHERIT, 0))
    {
        res.msg = "Stdout SetHandleInformation";
        res.status = -2;
        return res;
    }
    if (!CreatePipe(&in_read, &in_write, &security, 0))
    {
        res.msg = "Stdin CreatePipe";
        res.status = -3;
        return res;
    }
    if (!SetHandleInformation(in_write, HANDLE_FLAG_INHERIT, 0))
    {
        res.msg = "Stdin SetHandleInformation";
        res.status = -4;
        return res;
    }

    // Start target script
    STARTUPINFO startInfo;
    PROCESS_INFORMATION processInfo;

    ZeroMemory(&startInfo, sizeof(startInfo));
    startInfo.cb = sizeof(startInfo);
    startInfo.hStdError = out_write;
    startInfo.hStdOutput = out_write;
    startInfo.hStdInput = in_read;
    startInfo.dwFlags |= STARTF_USESTDHANDLES;

    ZeroMemory(&processInfo, sizeof(processInfo));

    // Write cin to program
    DWORD dwWrite;

    //CHAR inputBuf[1];
    //inputBuf[0] = 'a';
    WriteFile(in_write, testData, len, &dwWrite, NULL);

    // Start the child process
    // Documentation:
    // https://learn.microsoft.com/en-us/windows/win32/procthread/creating-processes
    if (!CreateProcess(
        path,           // Path to binary
        NULL,           // Command line
        NULL,           // Process handle not inheritable
        NULL,           // Thread handle not inheritable
        TRUE,           // Set handle inheritance
        0,              // No creation flags
        NULL,           // Use parent's environment block
        NULL,           // Use parent's starting directory 
        &startInfo,     // Pointer to STARTUPINFO
        &processInfo    // Pointer to PROCESS_INFORMATION
    ))
    {
        res.msg = "CreateProcess failed " + GetLastError();
        res.status = -5;
        return res;
    }

    // Wait until child process exits.
    WaitForSingleObject(processInfo.hProcess, INFINITE);

    // Close handles
    CloseHandle(processInfo.hProcess);
    CloseHandle(processInfo.hThread);
    CloseHandle(out_write);
    CloseHandle(in_read);

    // Get ouput
    DWORD dwRead;

    CHAR chBuf[BUFFSIZE + 1];
    while (ReadFile(out_read, chBuf, BUFFSIZE, &dwRead, NULL))
    {
        chBuf[dwRead] = 0;
        res.msg += chBuf;
    }

    CloseHandle(in_write);
    CloseHandle(out_read);

    return res;
}
