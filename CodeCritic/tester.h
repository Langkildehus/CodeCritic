#pragma once

#include "pch.h"

class Tester
{
public:
    Tester();
    ~Tester();

    void RunTest(const std::string& path) const;

private:
    void StartTest(const std::string& path) const;
    std::string Compile(const std::string& path) const;
    int Test(const std::string& compiledPath) const;
    void SaveScore(int score) const;
    void Delete(const std::string& compiledPath) const;
};
