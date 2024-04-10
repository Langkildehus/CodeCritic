#include "pch.h"

#include "tester.h"
#include "socket.h"
#include "database.h"

void HandleRequest(const SOCKET connection, const Tester& tester);
Database db; //create a instance of Database

int main()
{
	// Test users
	db.signup("SJJ", "1234");
	db.signup("SYJ", "1234");
	db.signup("ML", "1234");

	Tester tester = Tester(&db);
	//const std::string testFile = "Opgave1";
	//const std::string userName = "SJJ";
	//tester.RunTest(testFile, userName);

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
		std::thread handleThread(HandleRequest, connection, tester);
		handleThread.detach();
	}
}
