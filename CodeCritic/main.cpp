#include "pch.h"

#include "tester.h"
#include "socket.h"
#include "database.h"

void HandleRequest(const SOCKET connection);
Database db; //create a instance of Database

int main()
{
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
		std::thread handleThread(HandleRequest, connection);
		handleThread.detach();
	}
}
