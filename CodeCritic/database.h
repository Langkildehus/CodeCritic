#pragma once

#include "pch.h"

class Database 
{
public:
	Database();
	~Database();
	
	int createTable();
	int deleteData(std::string tName);
	int insertData();
	int updateData(std::string tName);
	int selectData(std::string tName);
	
private:
	//static int callback(void* NotUsed, int argc, char** argv, char** azColName);

	sqlite3* DB;
};