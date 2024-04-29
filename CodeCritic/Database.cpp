#include "pch.h"
#include "database.h"

// retrieve contents of database used by selectData()
/* argc: holds the number of results, argv: holds each value in array, azColName: holds each column returned in array, */
static int callback(void* NotUsed, int argc, char** argv, char** azColName)
{
	for (int i = 0; i < argc; i++) {
		// column name and value
		std::cout << azColName[i] << ": " << argv[i] << "\n";
	}

	std::cout << "\n";

	return 0;
}

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
	char* messageError;
	int ID;
	std::string sql = "INSERT INTO " + tName + " (Username, Points, Time) VALUES('" + username + "', '" + std::to_string(Points) + "', '" + std::to_string(time) + "');";
	
	int exit = sqlite3_exec(DB, sql.c_str(), NULL, 0, &messageError);
	if (exit != SQLITE_OK) {
		std::cerr << "Error in insertData function." << "\n" << messageError << "\n";
		sqlite3_free(messageError);
		
		std::string sql("UPDATE " + tName + " SET Points = '" + std::to_string(Points) + "', Time = '" + std::to_string(time) + "' WHERE Username = '" + username + "';");

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

int Database::selectData(const std::string& tName)
{
	char* messageError;

	std::string sql = "SELECT * FROM " + tName;

	/* An open database, SQL to be evaluated, Callback function, 1st argument to callback, Error msg written here*/
	int exit = sqlite3_exec(DB, sql.c_str(), callback, NULL, &messageError);

	if (exit != SQLITE_OK) {
		std::cerr << "Error in selectData function." << "\n" << messageError << "\n";
		sqlite3_free(messageError);
	}
	else
		std::cout << "Records selected Successfully!" << "\n";

	return 0;
}

std::string** Database::Assigmentleaderboard(const std::string tName) 
{
	//char* messageError;

	//std::string sql = "SELECT * FROM " + tName + "ORDER BY Points DESC";

	///* An open database, SQL to be evaluated, Callback function, 1st argument to callback, Error msg written here*/
	//int exit = sqlite3_exec(DB, sql.c_str(), callback, NULL, &messageError);

	//if (exit != SQLITE_OK) {
	//	std::cerr << "Error in selectData function." << "\n" << messageError << "\n";
	//	sqlite3_free(messageError);
	//}
	//else
	//	std::cout << "Records selected Successfully!" << "\n";

	//return 0;

	sqlite3_stmt* stmt;
	std::string sql = "SELECT Username FROM " + tName + " ORDER BY Points DESC;";
	std::vector<std::string> Leaderboard;
	int exit = sqlite3_prepare_v2(DB, sql.c_str(), -1, &stmt, NULL);

	while(true)
	{
		if (exit != SQLITE_OK || sqlite3_step(stmt) != SQLITE_ROW)
		{
			std::cout << "ERROR:" << sqlite3_errmsg(DB) << "\n";
			for (int c = 0; c < Leaderboard.size(); c++) 
			{
				std::cout << Leaderboard[c] << "\n";
			}

			return nullptr;
		}
	
	//Leaderboard.push_back(std::string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0))));
	Leaderboard.emplace_back(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0)));
	}

	//LB[]
}
