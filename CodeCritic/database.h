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
	int insertUserData(std::string name, std::string password);
	int selectData(std::string& tName);
	int updateData(std::string& tName);
	
private:

	sqlite3* DB;
};