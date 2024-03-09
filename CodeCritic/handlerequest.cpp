#include "pch.h"

void HandleRequest(SOCKET* connection)
{
	char buff[1024];
	int bytes = recv(*connection, buff, 1024, 0);

	std::stringstream ss(buff);
	std::string segment;
	std::vector<std::string> http;

	while (!std::getline(ss, segment, ' ').eof())
	{
		http.push_back(segment);
	}

	std::cout << http[0] << ' ' << http[1] << '\n';

	if (http[0] == "GET")
	{
		if (http[1] == "/api/opgaver")
		{
			std::string response = "[\"Opgave1\", \"Opgave2\", \"Grim\"]";
			std::cout << response << std::endl;
			send(*connection, response.c_str(), response.size(), 0);
			closesocket(*connection);
		}

		std::string path = "website" + http[1];

		// If path ends with '/', /index.html is assumed
		if (path[path.size() - 1] == '/')
		{
			path += "index.html";
		}

		// Verify that file is valid (And allowed??)

		send(*connection, "HTTP/1.1 200 OK\r\n\r\n", 19, 0);

		std::ifstream file(path);
		std::string line;
		while (std::getline(file, line))
		{
			send(*connection, line.c_str(), line.size(), 0);
		}
		file.close();
	}

	closesocket(*connection);
}
