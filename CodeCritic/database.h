#pragma once

#include "pch.h"

class Database 
{
public:
	Database();
	~Database();

	int createUsers();
	int createTable(std::string& tname);
	int deleteData(std::string& tName);
	int insertData();
	int insertUserData(std::string username, std::string password);
	int selectData(std::string& tName);
	int updateData(std::string& tName);
	bool checkLogin(std::string& username, std::string& Password);
	
private:

	sqlite3* DB;
};
