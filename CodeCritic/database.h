#pragma once

#include "pch.h"

class Database 
{
public:
	Database();
	~Database();

	int createTable(std::string& tname);
	int deleteData(std::string& tName);
	int insertData();
	bool signup(const std::string& username, const std::string& password);
	int selectData(std::string& tName);
	int updateData(std::string& tName, std::string& Points, std::string& username);
	bool checkLogin(std::string& username, std::string& Password);
	
private:

	sqlite3* DB;
};
