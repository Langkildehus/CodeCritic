#include "pch.h"
#define READSIZE 1024

void HandlePOST(std::string& msg)
{

}

void HandleRequest(SOCKET connection)
{
	// The HTTP header needs to be read first, before we know the length of the body - if there is a body
	// Create buffer for reading the socket & string to save header in
	char buff[READSIZE];
	std::string req = "";

	// overRead keeps track of how many bytes into a potential HTTP body we have read
	int overRead = 0;

	// Keep track of last three characters read to look for CRLF (\r\n\r\n)
	char prev[3] = { '0', '0', '0' };

	// Read first section of bytes from the windows socket
	int bytes = recv(connection, buff, READSIZE, 0);
	while (bytes)
	{
		bool CRLF = false;

		// Read one character at a time from the buffer
		for (int c = 0; c < bytes; c++)
		{
			// Read untill we find CRLF (\r\n\r\n)
			if (prev[0] == '\r' && prev[1] == '\n' && prev[2] == '\r' && buff[c] == '\n')
			{
				// CRLF found -> end of header
				// Update overRead to the amount of bytes left in the buffer for a potential body
				overRead = READSIZE - c - 1;
				CRLF = true;
				break;
			}

			// Update prev three characters
			prev[0] = prev[1];
			prev[1] = prev[2];
			prev[2] = buff[c];

			// Save current character to header
			req += buff[c];
		}

		if (CRLF)
		{
			// If CRLF is found, we are done reading the HTTP header
			// We can't read the body yet, since there is no guarantee for a body to sent with the request
			break;
		}

		// Read next section of bytes
		bytes = recv(connection, buff, READSIZE, 0);
	}

	// Now that the header has been read, we need to parse it
	std::stringstream ss(req);
	std::string segment;
	std::vector<std::string> header;

	// If there is a body, save it's length
	int contentLength = 0;

	// Go through every line in the header
	while (!std::getline(ss, segment, '\n').eof())
	{
		// Save the header where every element in the vector is a new line
		header.push_back(segment);

		// Check if we find "Content-Length" in the header
		// If it is never found, no body has been sent with the request
		if (header.size() > 1 && header[header.size() - 1].substr(0, 16) == "Content-Length: ")
		{
			// Read the value of "Content-Length" and save it
			contentLength = std::stoi(segment.substr(16));
		}
	}

	if (header.size() < 2)
	{
		std::cout << "Ignoring bad http request\n";
		return;
	}

	// Parse the first line of the header, since it contains multiple important pieces of information
	std::stringstream generalHeader(header[0]);
	std::string type;
	std::string url;
	int c = 0;
	while (!std::getline(generalHeader, segment, ' ').eof())
	{
		if (c == 0)
		{
			// First word is the type of request (GET, POST...)
			type = segment;
		}
		else if (c == 1)
		{
			// Parse the second word as the requested url

			int i = 0;
			for (char a : segment)
			{
				url += a;
				if (i > 2)
				{
					if (url[i - 2] == '%' && url[i - 1] == '2' && url[i] == '0')
					{
						std::cout << "SPACE FOUND\n";
						url.pop_back();
						url.pop_back();
						url[i - 2] = ' ';
						i -= 2;
					}
				}
				i++;
			}
		}
		else
		{
			// Last word is the HTTP version
			if (segment != "HTTP/1.1")
			{
				std::cout << "Ignoring request. Expected HTTP/1.1\n";
				closesocket(connection);
				return;
			}
		}
		c++;
	}

	std::cout << type << ':' << url << "\n";

	if (type == "GET")
	{
		// Check if GET request is requesting data or website
		if (url.size() > 3 && url.substr(0, 4) == "/api")
		{
			// Read data for request
			std::string data = "{\"Opgaver\": [\"Opgave1\", \"Opgave2\", \"Grim\"]}";
			std::string response = "HTTP/1.1 200 OK\nContent-Type: application/json\nContent - Length: 43\r\n\r\n";

			// Send response header
			send(connection, response.c_str(), response.size(), 0);

			// Send response body
			send(connection, data.c_str(), data.size(), 0);
			closesocket(connection);
			return;
		}

		// If we reach this point, the website is requested
		// Generate path to requested file
		std::string path = "website" + url;

		// If path ends with '/', /index.html is assumed
		if (path[path.size() - 1] == '/')
		{
			path += "index.html";
		}

		// Verify that file is valid (And allowed??)

		// Send response header
		send(connection, "HTTP/1.1 200 OK\r\n\r\n", 19, 0);

		// Create stream to read requested file
		std::ifstream file(path);
		std::string line;
		// Read line by line
		while (std::getline(file, line))
		{
			// Manually add newline character at the end of every new line since they are not read from the file
			line += "\n";

			// Send requested as part of the body
			send(connection, line.c_str(), line.size(), 0);
		}

		// Close filestream and socket connection
		file.close();
		closesocket(connection);
		return;
	}
	else if (type == "POST")
	{
		if (contentLength <= 0)
		{
			// Ignore POST with no body
			std::cout << "Bad POST request\n";
			closesocket(connection);
			return;
		}

		// Create string for body
		std::string body = "";

		// If some of the body has been read while reading the header, extract if from the buffer here
		if (contentLength > 0 && overRead > 0)
		{
			// Loop through the remainder of the buffer
			for (int i = READSIZE - overRead; i < READSIZE; i++)
			{
				if (i >= READSIZE - overRead + contentLength)
				{
					break;
				}
				body += buff[i];
			}
		}

		// Update remaining contenLength to be the amount left to read from socket
		contentLength -= overRead;
		while (contentLength > 0)
		{
			//Read from socket
			bytes = recv(connection, buff, contentLength > READSIZE ? READSIZE : contentLength, 0);
			contentLength -= READSIZE;

			// Go through all characters read and save them
			for (int i = 0; i < bytes; i++)
			{
				body += buff[i];
			}
		}

		HandlePOST(body);
		closesocket(connection);
		return;
	}

	// Ignore everything else
	closesocket(connection);
}
