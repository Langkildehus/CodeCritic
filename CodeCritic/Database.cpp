#include "pch.h"
#include "database.h"

Database::Database()
{
	int exit = sqlite3_open(R"(Database.db)", &DB);
	std::string sql = "CREATE TABLE IF NOT EXISTS Users("
		"ID			INTEGER PRIMARY KEY AUTOINCREMENT, "
		"USERNAME   TEXT UNIQUE NOT NULL, "
		"PASSWORD	TEXT NOT NULL);";
	exit = sqlite3_exec(DB, sql.c_str(), NULL, 0, NULL);
}

Database::~Database()
{
	sqlite3_close(DB);
}

int Database::createAssignment(const std::string& tName)
{
	char* messageError;
	std::string sql;
	sql = "CREATE TABLE IF NOT EXISTS " + tName + "("
		"ID			INTEGER PRIMARY KEY AUTOINCREMENT, "
		"Username   STRING UNIQUE NOT NULL, "
		"Points		INTEGER NOT NULL, "
		"Time		INTEGER NOT NULL); ";


	try
	{
		/* An open database, SQL to be evaluated, Callback function, 1st argument to callback, Error msg written here */
		int exit = sqlite3_exec(DB, sql.c_str(), NULL, 0, &messageError);
		if (exit != SQLITE_OK) {
			std::cerr << "Error in createTable function\n" << messageError << "\n";
			sqlite3_free(messageError);
		}
		else
			std::cout << "Table created Successfully" << "\n";
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what();
	}

	return 0;
}

int Database::insertData(const std::string& tName, const std::string& username, const int Points, const ull time)
{
	sqlite3_stmt* stmt;
	char* messageError;
	std::string sql = "INSERT INTO " + tName + " (Username, Points, Time) VALUES('" + username + "', '" + std::to_string(Points) + "', '" + std::to_string(time) + "');";
	
	int exit = sqlite3_exec(DB, sql.c_str(), NULL, 0, &messageError);
	if (exit != SQLITE_OK) {
		std::cerr << "Error in insertData function." << "\n" << messageError << "\n";
		sqlite3_free(messageError);
		
		sql = "SELECT Points, Time FROM " + tName + " Where Username is '" + username + "' ;";
		exit = sqlite3_prepare_v2(DB, sql.c_str(), -1, &stmt, NULL);

		if (exit != SQLITE_OK || sqlite3_step(stmt) != SQLITE_ROW)
		{
			std::cout << "ERROR:" << sqlite3_errmsg(DB) << "\n";
			return 0;
		}
		std::stringstream tempPoints, tempTime;
		int DBpoints, DBtime;
		tempPoints << sqlite3_column_text(stmt, 0);
		tempPoints >> DBpoints;
		tempTime << sqlite3_column_text(stmt, 1);
		tempTime >> DBtime;
		std::cout << DBpoints << "\n" << DBtime << "\n";
		if (DBpoints < Points || (DBpoints == Points && DBtime > time))
		{
			sql = "UPDATE " + tName + " SET Points = '" + std::to_string(Points) + "', Time = '" + std::to_string(time) + "' WHERE Username = '" + username + "';";

			int exit = sqlite3_exec(DB, sql.c_str(), NULL, 0, &messageError);
			if (exit != SQLITE_OK)
			{
				std::cerr << "Error in updateData function." << "\n" << messageError << "\n";
				sqlite3_free(messageError);
			}
			else
				std::cout << "Records updated Successfully!" << "\n";
			return 0;
		}
		std::cout << "updata not needed" << "\n";
		return 0;
	}
	else
		std::cout << "Records inserted Successfully!" << "\n";
		return 0;
}

bool Database::signup(const std::string& username, const std::string& password)
{
	char* messageError;

	std::string sql("INSERT INTO Users (USERNAME, PASSWORD) VALUES('"+ username + "','" + password +"');");


	 //An open database, SQL to be evaluated, Callback function, 1st argument to callback, Error msg written here 
	int exit = sqlite3_exec(DB, sql.c_str(), NULL, 0, &messageError);
	if (exit != SQLITE_OK) {
		std::cerr << "Error in insertUserData function." << "\n" << messageError << "\n";
		sqlite3_free(messageError);
		return false;
	}
	else
		std::cout << "Records inserted Successfully!" << "\n";

	return true;
	
}

bool Database::checkLogin(const std::string& username, const std::string& Password)
{
	sqlite3_stmt* stmt;
	std::string sql = "SELECT PASSWORD FROM Users WHERE USERNAME= '" + username + "';";
	int exit = sqlite3_prepare_v2(DB, sql.c_str(), -1, &stmt, NULL);

	if (exit != SQLITE_OK|| sqlite3_step(stmt) != SQLITE_ROW)
	{
		std::cout << "ERROR:" << sqlite3_errmsg(DB) << "\n";
		return false;
	}
	if (Password == std::string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0))))
	{
		std::cout << "win\n";
		return true;
	}
	std::cout << "fail\n";
	return false;
}

std::string Database::Assigmentleaderboard(const std::string tName) 
{
	sqlite3_stmt* stmt;
	std::string sql = "SELECT Username, Points, Time FROM " + tName + " ORDER BY Points DESC, Time ASC;";
	std::string str = "[";
	int exit = sqlite3_prepare_v2(DB, sql.c_str(), -1, &stmt, NULL);


	while(true)
	{
		if (exit != SQLITE_OK || sqlite3_step(stmt) != SQLITE_ROW)
		{
			std::cout << "ERROR:" << sqlite3_errmsg(DB) << "\n";
			str[str.size() - 1] = ']';
			std::cout << str;
			return str;
		}
		str += '{';

		str += "\"name\": \"" + std::string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0))) + "\",";
		str += "\"points\": " + std::string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1))) + ',';
		str += "\"time\": " + std::string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2)));

		str += " },";
	}
}
