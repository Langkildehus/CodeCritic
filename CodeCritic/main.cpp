#include "pch.h"

#include "tester.h"
#include "socket.h"
#include "database.h"

#define AVX512

void HandleRequest(const SOCKET connection, Tester* tester);
Database db{}; //create a instance of Database

void SetupAssignments()
{
	const std::string path = "website\\opgaver\\";
	for (const std::filesystem::directory_entry& entry : std::filesystem::directory_iterator("website/opgaver"))
	{
		// Only look for directories
		if (!entry.is_directory())
		{
			continue;
		}

		// Find directory name
		const std::string dir = entry.path().string();
		const size_t folderIndex = dir.find_last_of("\\");
		const std::string assignment = dir.substr(folderIndex + 1);

		// Make sure there is a table for every task
		db.createAssignment(assignment);

		const std::string compilePath = path + assignment + "\\judge.";
		const std::string cmd = "..\\MinGW\\bin\\g++.exe --std=c++17 -O3 -mavx2 " + compilePath + "cpp -o " + compilePath + "exe";
		std::system(cmd.c_str());
	}
}

int main()
{
	// Default DB & Assignment setup
	SetupAssignments();
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
