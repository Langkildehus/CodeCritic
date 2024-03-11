#include "pch.h"

#include "tester.h"
#include "socket.h"

void HandleRequest(SOCKET* connection);

int main()
{
	//const std::string testFile = "C:\\dev\\CodeCritic\\x64\\Debug\\Testscript.exe";
	const std::string testFile = "TestScript";

	Tester tester{};
	tester.RunTest(testFile);

	// Start listening on webserver
	Socket server = Socket("10.130.148.1", 80);
	server.Listen(5);

	while (true)
	{
		// Accept incoming connection
		SOCKET* connection = server.Accept();
		if (connection == NULL)
		{
			std::cout << "Connection failed\n";
			continue;
		}

		HandleRequest(connection);
	}
}
