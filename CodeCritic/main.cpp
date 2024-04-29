#include "pch.h"

#include "tester.h"
#include "socket.h"
#include "database.h"

void HandleRequest(const SOCKET connection, Tester* tester);
Database db{}; //create a instance of Database

void CreateAssignmentTables()
{
	for (const std::filesystem::directory_entry& entry : std::filesystem::directory_iterator("website/opgaver"))
	{
		// Only look for directories
		if (!entry.is_directory())
			continue;

		// Find directory name
		const std::string dir = entry.path().string();
		const size_t folderIndex = dir.find_last_of("\\");

		// Make sure there is a table for every task
		db.createAssignment(dir.substr(folderIndex + 1));
	}
}

int main()
{
	// Default DB setup
	CreateAssignmentTables();
	db.signup("SJJ", "1234");
	db.signup("SYJ", "1234");
	db.signup("ML", "1234");

	// Instantiate Tester
	Tester tester{};

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
		std::thread handleThread(HandleRequest, connection, &tester);
		handleThread.detach();
	}
}
