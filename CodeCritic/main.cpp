#include "pch.h"

#include "tester.h"
#include "socket.h"
#include "database.h"

void HandleRequest(const SOCKET connection, Tester* tester);
Database db{}; //create a instance of Database

struct Config
{
	std::string IP;
	int PORT;
	int QUEUE;
};

Config Load()
{
	std::string ip;
	int port, queue;

	std::ifstream f("config.txt");
	f >> ip >> port >> queue;
	f.close();

	return { ip, port, queue };
}

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
		const std::string cmd = "g++.exe --std=c++17 -O3 -mavx2 " + compilePath + "cpp -o " + compilePath + "exe";
		std::system(cmd.c_str());
	}
}

int main()
{
	// Default DB & Assignment setup
	SetupAssignments();

	// Instantiate Tester
	Tester tester{};

	// Load server config
	const Config cfg = Load();

	// Start listening on webserver
	Socket server = Socket(cfg.IP, cfg.PORT);
	server.Listen(cfg.QUEUE);

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
