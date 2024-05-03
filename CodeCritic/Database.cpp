#include "pch.h"
#include "database.h"

//the database constructor
Database::Database()
{
	//opens the database so it can be accessed
	int exit = sqlite3_open(R"(Database.db)", &DB);

	//creates a table in the data base for useres if it doesen't already exist (only needed if the database has been reset)
	std::string sql = "CREATE TABLE IF NOT EXISTS Users("
		"ID			INTEGER PRIMARY KEY AUTOINCREMENT, "
		"USERNAME   TEXT UNIQUE NOT NULL, "
		"PASSWORD	TEXT NOT NULL);";
	exit = sqlite3_exec(DB, sql.c_str(), NULL, 0, NULL);
}

//the database destructor
Database::~Database()
{
	//closes the database
	sqlite3_close(DB);
}

//function to create a table for an assignment so there can be keept track of which users scores what
int Database::createAssignment(const std::string& tName)
{
	//makes a table in the database with the name from the input if it doesen't already exist
	char* messageError;
	std::string sql;
	sql = "CREATE TABLE IF NOT EXISTS " + tName + "("
		"ID			INTEGER PRIMARY KEY AUTOINCREMENT, "
		"Username   STRING UNIQUE NOT NULL, "
		"Points		INTEGER NOT NULL, "
		"Time		INTEGER NOT NULL, "
		"Language	STRING NOT NULL); ";

		int exit = sqlite3_exec(DB, sql.c_str(), NULL, 0, &messageError);

		//checkes if the table is created correct if not it prints in the terminal what went wrong
		if (exit != SQLITE_OK) 
		{
			std::cerr << "Error in createTable function\n" << messageError << "\n";
			sqlite3_free(messageError);
		}
		//if it created the table correct prints in the terminal that it worked 
		else
		{
			std::cout << "Table created Successfully" << "\n";
		}
	return 0;
}

//function to insert data into the table of the assignment that has been tried
int Database::insertData(const std::string& tName, const std::string& username, const int Points, const ull time, const std::string& language)
{
	//inserts the scores a user gets if the user has not already a saved score
	sqlite3_stmt* stmt;
	char* messageError;
	std::string sql = "INSERT INTO " + tName + " (Username, Points, Time, Language) VALUES('" + username + "', '" + std::to_string(Points) + "', '" + std::to_string(time) + "', '" + language + "'); ";
	
	int exit = sqlite3_exec(DB, sql.c_str(), NULL, 0, &messageError);

	//if the user already has a scores in the database then try and update it
	if (exit != SQLITE_OK) {
		std::cerr << "Error in insertData function." << "\n" << messageError << "\n";
		sqlite3_free(messageError);
		//get the previous score and time to check if the new is better
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

		//check if the new score is better or the new score is the same and the time is better 
		if (DBpoints < Points || (DBpoints == Points && DBtime > time))
		{
			//then update the saved score
			sql = "UPDATE " + tName + " SET Points = '" + std::to_string(Points) + "', Time = '" + std::to_string(time) + "', Language = '" + language + "' WHERE Username = '" + username + "'; ";

			int exit = sqlite3_exec(DB, sql.c_str(), NULL, 0, &messageError);

			if (exit != SQLITE_OK)
			{
				std::cerr << "Error in updateData." << "\n" << messageError << "\n";
				sqlite3_free(messageError);
			}
			else
				std::cout << "Records updated." << "\n";
			return 0;
		}
		//if the new score is worse then the previous the do not update the saved score
		std::cout << "The records do not need to be updated" << "\n";
		return 0;
	}
	else
		std::cout << "Records inserted." << "\n";
		return 0;
}

//function to signup (create a new user)
bool Database::signup(const std::string& username, const std::string& password)
{
	//makes a new user with the username and password from the input if there is not already a user with the same username
	char* messageError;
	std::string sql("INSERT INTO Users (USERNAME, PASSWORD) VALUES('"+ username + "','" + password +"');");

	int exit = sqlite3_exec(DB, sql.c_str(), NULL, 0, &messageError);

	//checks if the worked other wises prints what went wrong in the terminal and returns that it did not work
	if (exit != SQLITE_OK) {
		std::cerr << "Error in insertUserData function." << "\n" << messageError << "\n";
		sqlite3_free(messageError);
		return false;
	}
	//returns that it worked and prints in the terminal that it worked
	else
	{
		std::cout << "Records inserted Successfully!" << "\n";
		return true;
	}
}

//function to login (access a user that already exists)
bool Database::checkLogin(const std::string& username, const std::string& Password)
{
	//to login the password you try to login with and the login that the user has need to be the same
	//gets the password from the database
	sqlite3_stmt* stmt;
	std::string sql = "SELECT PASSWORD FROM Users WHERE USERNAME= '" + username + "';";
	int exit = sqlite3_prepare_v2(DB, sql.c_str(), -1, &stmt, NULL);

	//check if it was possible to get the password from the database. 
	//if not, print the the error to the terminal and return false to indicate that the login failed
	if (exit != SQLITE_OK|| sqlite3_step(stmt) != SQLITE_ROW)
	{
		std::cout << "ERROR:" << sqlite3_errmsg(DB) << "\n";
		return false;
	}
	//then check if the password from the database is the same as the one that the user tried
	//if it is the same then return that the login worked otherwise return that it did not work
	if (Password == std::string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0))))
	{
		std::cout << "win\n";
		return true;
	}
	std::cout << "fail\n";
	return false;
}

//function to get a leaderboard over which useres made the best and fastest solution
std::string Database::Assigmentleaderboard(const std::string tName) 
{
	//get the users and scores from the given assigment
	//in order of which users that got most points and used least amount of time
	sqlite3_stmt* stmt;
	std::string sql = "SELECT Username, Points, Time, Language FROM " + tName + " ORDER BY Points DESC, Time ASC;";
	std::string str = "[";
	int exit = sqlite3_prepare_v2(DB, sql.c_str(), -1, &stmt, NULL);

	//Go through all the users and scores one by one and write them in a string that follows a JSON format 
	//so it can be sent to the http server
	while(true)
	{
		//check if there is more scores left 
		if (exit != SQLITE_OK || sqlite3_step(stmt) != SQLITE_ROW)
		{
			//if there is no more scores left then add the end of a JSON file to it
			//and return it
			str[str.size() - 1] = ']';
			std::cout << str << "\n";
			return str;
		}
		//write the user and the users scores in a string so it follow JSON format
		str += '{';

		str += "\"name\": \"" + std::string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0))) + "\",";
		str += "\"points\": " + std::string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1))) + ',';
		str += "\"time\": " + std::string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2))) + ',';
		str += "\"language\": \"" + std::string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3))) + "\"";
		

		str += " },";
	}
}
