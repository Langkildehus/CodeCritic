#include "pch.h"

#include "tester.h"
#include "socket.h"

void HandleRequest(const SOCKET connection);

int main()
{
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
