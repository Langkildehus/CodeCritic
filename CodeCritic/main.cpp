#include "pch.h"

#include "tester.h"
#include "socket.h"
#include "database.h"

void HandleRequest(SOCKET connection);

int main()
{
	
	Database db; //create a inctens of Database
	std::string str = "Users";
	db.createTable(str);
	std::string n1 = "hej1";
	std::string p = "Magnus";
	std::string n2 = "hej2";
	db.insertUserData(n1, p);
	db.insertUserData(n2, p);
	db.insertUserData(n1, p);
	//const std::string testFile = "C:\\dev\\CodeCritic\\x64\\Debug\\Testscript.exe";
	const std::string testFile = "TestScript";

	Tester tester{};
	//tester.RunTest(testFile);

	// Start listening on webserver
	Socket server = Socket("127.0.0.1", 80);
	server.Listen(5);

	while (true)
	{
		// Accept incoming connection
		SOCKET connection = server.Accept();
		if (connection == NULL)
		{
			std::cout << "Connection failed\n";
			continue;
		}

		std::thread handleThread(HandleRequest, connection);
		handleThread.detach();
	}
}
