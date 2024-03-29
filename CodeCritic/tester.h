#pragma once

#include "pch.h"

class Tester
{
public:
    Tester();
    ~Tester();

    void RunTest(const std::string& name) const;

private:
    void StartTest(const std::string& path) const;
    std::string Compile(const std::string& path) const;
    void SaveScore(int score) const;
    void Delete(const std::string& compiledPath) const;

    // The test itself
    int Test(const std::string& compiledPath) const;
    bool WriteToPipe(const std::string& msg, const HANDLE& pipeHandle);
    void ReadFromPipe(const HANDLE& pipeHandle, std::vector<std::string>& vec);
};
