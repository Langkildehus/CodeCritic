#pragma once

#include <sstream>
#include <Windows.h>

struct Result
{
    int status = 0;
    std::string msg;
};

Result RunTest(std::string file, CHAR* testData, size_t len);
