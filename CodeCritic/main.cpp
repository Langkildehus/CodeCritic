#include "pch.h"

#include "tester.h"
#include "socket.h"
#include "database.h"

void HandleRequest(const SOCKET connection);
Database db; //create a instance of Database

int main()
{

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
