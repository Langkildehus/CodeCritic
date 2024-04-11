#pragma once

#include "pch.h"

class Database 
{
public:
	Database();
	~Database();

	int createTable(const std::string& tName);
	int insertData(const std::string& tName, const int Points, const std::string& username);
	int deleteData(std::string& tName);
	bool signup(const std::string& username, const std::string& password);
	int selectData(const std::string& tName);
	bool checkLogin(std::string& username, std::string& Password);
	
private:

	sqlite3* DB;
};
