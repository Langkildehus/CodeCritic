#pragma once

typedef unsigned int uint;
typedef unsigned long long ull;

#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <fstream>
#include <thread>
#include <utility>
#include <cstdio>
#include <stdlib.h>
#include <filesystem>
#include <chrono>

// Windows includes
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <Windows.h>

// SQlite includes
#include "stdio.h"
#include "sqlite3.h"

struct Cookie
{
	std::string username = "";
	std::string assignment = "";
};
