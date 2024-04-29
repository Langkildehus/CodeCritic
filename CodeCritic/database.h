#pragma once

#include "pch.h"

class Database 
{
public:
	Database();
	~Database();

	int createAssignment(const std::string& tName);
	int insertData(const std::string& tName, const std::string& username, const int Points, const ull time);
	bool signup(const std::string& username, const std::string& password);
	int selectData(const std::string& tName);
	bool checkLogin(const std::string& username, const std::string& Password);
	std::string Assigmentleaderboard(const std::string tName);
	//std::string* leaderboard();
	
private:

	sqlite3* DB;
};
