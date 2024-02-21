#pragma once

#include "pch.h"

struct Result
{
    int status = 0;
    int result = 0;
    std::string msg;
};

Result RunTest(
    std::string file,
    CHAR* testData,
    size_t len
);

Result RunInteractive(
    std::string file,
    int timeLimit,
    int (*Test)(std::vector<std::string>& inputs, std::vector<std::string>& outputs)
);
