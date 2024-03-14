#include "pch.h"
#define READSIZE 1024

void HandleRequest(SOCKET* connection)
{
	std::string req = "";

	// Read untill '\r\n\r\n'
	char buff[READSIZE];
	int overRead = 0;
	int bytes = recv(*connection, buff, READSIZE, 0);
	char prev[3] = { '0', '0', '0' };
	while (bytes)
	{
		bool CRLF = false;
		for (int c = 0; c < bytes; c++)
		{
			// Check for '\r\n\r\n'
			if (prev[0] == '\r' && prev[1] == '\n' && prev[2] == '\r' && buff[c] == '\n')
			{
				overRead = READSIZE - c - 1;
				CRLF = true;
				break;
			}

			// Update prev
			prev[0] = prev[1];
			prev[1] = prev[2];
			prev[2] = buff[c];
			req += buff[c];
		}

		if (CRLF)
		{
			break;
		}

		bytes = recv(*connection, buff, READSIZE, 0);
	}

	std::stringstream ss(req);
	std::string segment;
	std::vector<std::string> header;
	int contentLength = 0;

	while (!std::getline(ss, segment, '\n').eof())
	{
		header.push_back(segment);
		if (header.size() > 1 && header[header.size() - 1].substr(0, 16) == "Content-Length: ")
		{
			contentLength = std::stoi(segment.substr(16));
		}
		//std::cout << "\n--------------\n" << segment;
	}
	if (header.size() < 2)
	{
		std::cout << "Ignoring bad http request\n";
		return;
	}

	std::stringstream generalHeader(header[0]);

	std::string type;
	std::string url;

	int c = 0;
	while (!std::getline(generalHeader, segment, ' ').eof())
	{
		if (c == 0)
		{
			type = segment;
		}
		else if (c == 1)
		{
			url = segment;
		}
		else
		{
			if (segment != "HTTP/1.1")
			{
				std::cout << "Ignoring request. Expected HTTP/1.1\n";
				closesocket(*connection);
				return;
			}
		}
		c++;
	}

	if (type == "GET")
	{
		// Check if GET request is requesting data from api
		if (url.size() > 3 && url.substr(0, 4) == "/api")
		{
			std::string data = "{\"Opgaver\": [\"Opgave1\", \"Opgave2\", \"Grim\"]}";
			std::string response = "HTTP/1.1 200 OK\nContent-Type: application/json\nContent - Length: 43\r\n\r\n";

			send(*connection, response.c_str(), response.size(), 0);
			send(*connection, data.c_str(), data.size(), 0);
			closesocket(*connection);
			return;
		}

		// Generate path to file
		std::string path = "website" + url;

		// If path ends with '/', /index.html is assumed
		if (path[path.size() - 1] == '/')
		{
			path += "index.html";
		}

		// Verify that file is valid (And allowed??)

		// Send response header
		send(*connection, "HTTP/1.1 200 OK\r\n\r\n", 19, 0);

		// Create stream to open file
		std::ifstream file(path);
		std::string line;
		while (std::getline(file, line))
		{
			line += "\n";
			// Send requested file as reponse body
			send(*connection, line.c_str(), line.size(), 0);
		}
		file.close();
		closesocket(*connection);
		return;
	}
	else if (type == "POST")
	{
		int remaining = contentLength - overRead;
		std::string body = "";

		if (contentLength > 0 && overRead > 0)
		{
			for (int i = READSIZE - overRead; i < READSIZE; i++)
			{
				if (i >= READSIZE - overRead + contentLength)
				{
					break;
				}
				body += buff[i];
			}
		}
		contentLength -= overRead;

		while (contentLength > 0)
		{
			bytes = recv(*connection, buff, contentLength > READSIZE ? READSIZE : contentLength, 0);
			contentLength -= READSIZE;

			for (int i = 0; i < bytes; i++)
			{
				body += buff[i];
			}
		}
		std::cout << body << "\n";
	}

	// Ignore everything else
	closesocket(*connection);
}
