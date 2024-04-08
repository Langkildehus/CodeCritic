#include "pch.h"

#include "tester.h"
#include "socket.h"
#include "database.h"

void HandleRequest(const SOCKET connection);
Database db; //create a instance of Database

int main()
{
	
	std::string str = "Users";
	db.createUsers();
	std::string n1 = "hej1";
	std::string p = "Magnus";
	std::string n2 = "hej2";
	
	db.checkLogin(n1, p);

	//const std::string testFile = "C:\\dev\\CodeCritic\\x64\\Debug\\Testscript.exe";
	std::string testFile = "TestScript";

	Tester tester{};
	tester.RunTest(std::move(testFile));

	// Start listening on webserver
	Socket server = Socket("127.0.0.1", 80);
	server.Listen(5);

	while (true)
	{
		// Accept incoming connection
		const SOCKET connection = server.Accept();
		if (connection == NULL)
		{
			std::cout << "Connection failed\n";
			continue;
		}

		// Create thread for connection and detatch it
		std::thread handleThread(HandleRequest, connection);
		handleThread.detach();
	}
}
