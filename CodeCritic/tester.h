#pragma once

#include "pch.h"

#include "database.h"

class Tester
{
public:
    Tester(Database* _db);
    ~Tester();

    void RunTest(const std::string& task, const std::string& name) const;

private:
    void StartTest(const std::string task, const std::string name) const;
    std::string Compile(const std::string& path) const;
    void SaveScore(const int score) const;
    void Delete(const std::string& compiledPath) const;

    // The test itself
    int Test(const LPCWSTR& judgePath, const LPCWSTR& testPath, const std::string& testData, const ull timeLimit) const;
    void WriteToPipe(const HANDLE& pipeHandle, const std::string& msg) const;
    bool ReadFromPipe(const HANDLE& pipeHandle, std::vector<std::string>& vec, const bool waitForData = false) const;

    // Pointer to database
    Database* db;
};
