#pragma once

#include "pch.h"

#include "database.h"

struct Result
{
    int status = 0;
    int points = 0;
    ull time = 0;
};

class Tester
{
public:
    Tester();
    ~Tester();

    void RunTest(const Cookie& cookies) const;

private:
    void StartTest(const std::string task, const std::string name) const;
    inline std::string Compile(const std::string& path) const;
    inline void SaveScore(const std::string& task, const std::string& name, const int points, const ull time) const;
    inline void Delete(const std::string& compiledPath) const;

    // The test itself
    Result Test(const LPCWSTR& judgePath, const LPCWSTR& testPath, const std::string& testData, const int timeLimit) const;
    void WriteToPipe(const HANDLE& pipeHandle, const std::string& msg) const;
    bool ReadFromPipe(const HANDLE& pipeHandle, std::vector<std::string>& vec, const bool waitForData = false) const;
};
