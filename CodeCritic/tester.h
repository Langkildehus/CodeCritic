#pragma once

#include "pch.h"

#include "database.h"

struct Result
{
    int status = 0;
    int points = 0;
    ull time = 0;
};

struct WriteThreadData
{
    const HANDLE& pipeHandle;
    std::mutex mtx;
    std::condition_variable cnd;
    std::string* str = nullptr;
    bool go = true;
};

class Tester
{
public:
    Tester();
    ~Tester();

    void RunTest(const Cookie& cookies);

private:
    void StartTest(const std::string task, const std::string name);
    inline std::string Compile(const std::string& path) const;
    inline void SaveScore(const std::string& task, const std::string& name, const int points, const ull time) const;
    inline void Delete(const std::string& compiledPath) const;

    // The test itself
    Result Test(const LPCWSTR& judgePath, const LPCWSTR& testPath, std::string& testData, const int timeLimit) const;
    void WriteToPipe(const HANDLE& pipeHandle, const std::string& msg) const;
    bool ReadFromPipe(const HANDLE& pipeHandle, std::vector<std::string>& vec) const;
    inline void CleanFromENDL(std::string& str) const;
    void WriteThread(WriteThreadData* threadData) const;

private:
    std::mutex runMtx;
};