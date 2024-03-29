#include "pch.h"
#define READSIZE 1024

bool FindTaskName(std::string& path, std::string& name)
{
	// Look for "opgaver" in path
	const size_t index = path.find("opgaver");
	if (index == std::string::npos || index + 8 > path.size())
	{
		return false;
	}

	// Look for next '/'
	size_t end = path.find('/', index + 8);
	if (end == std::string::npos)
	{
		end = path.size();
	}

	// Save name of task and redirect path to the generic html file
	name = path.substr(index + 8, end - index - 8);
	path = path.substr(0, index) + "opgave.html";
	return true;
}

void ReplaceInLine(std::string& line, const std::vector<std::string>& description)
{
	size_t pos = line.find("{{");
	size_t end = line.find("}}");
	while (pos != std::string::npos && end != std::string::npos)
	{
		const std::string replace = line.substr(pos + 2, end - pos - 2);
		if (replace == "Title")
		{
			line.replace(line.begin() + pos, line.begin() + end + 2, description[0]);
		}
		else if (replace == "Description")
		{
			std::string insert = description[1];
			for (int c = 2; c < description.size(); c++)
			{
				insert += "\n" + description[c];
			}
			line.replace(line.begin() + pos, line.begin() + end + 2, insert);
		}

		pos = line.find("{{", end + 2);
		end = line.find("}}", end + 2);
	}
}

void HandleGET(const SOCKET connection, const std::string& url)
{
	// Check if GET request is requesting list of tasks
	if (url.size() == 8 && url.substr(0, 8) == "/opgaver")
	{
		// Read folders in directory
		std::string data = "{\"Opgaver\": [";
		for (const std::filesystem::directory_entry& entry : std::filesystem::directory_iterator("website/opgaver"))
		{
			// Only look for directories
			if (!entry.is_directory())
				continue;

			// Find directory name
			const std::string dir = entry.path().string();
			const size_t folderIndex = dir.find_last_of("\\");

			// Append directory (task) to the string as JSON
			data += '"' + dir.substr(folderIndex + 1) + "\", ";
		}
		data.pop_back(); data.pop_back(); // Remove last two characters: ", "
		data += "]}";

		// Create response header
		const std::string response = "HTTP/1.1 200 OK\nContent-Type: application/json\nContent - Length: "
			+ std::to_string(data.size()) + "\r\n\r\n";

		// Send response header
		send(connection, response.c_str(), response.size(), 0);

		// Send response body
		send(connection, data.c_str(), data.size(), 0);
		return;
	}

	// Generate path to requested file. Paths that end with '/' assume /index.html
	std::string name, path = (url[url.size() - 1] == '/' ? "website" + url + "index.html" : "website" + url);

	// Check if lines need to be replaced in sent file
	const bool isDynamic = FindTaskName(path, name);

	// If the file is dynamic, read the input to the dynamic file
	std::vector<std::string> description;
	if (isDynamic)
	{
		// Open file as an input file stream
		std::ifstream dynamicFile("website/opgaver/" + name + "/description.txt");
		std::string line;
		while (std::getline(dynamicFile, line))
		{
			// Save all lines in a vector
			line += '\n';
			description.push_back(line);
		}
		dynamicFile.close();

		if (description.size() < 2)
		{
			// If an invalid dynamic file is requested, reject request
			send(connection, "HTTP/1.1 403 Forbidden\r\n\r\n", 26, 0);
			return;
		}
	}
	
	// Send response header
	send(connection, "HTTP/1.1 200 OK\r\n\r\n", 19, 0);

	// Create input file stream to read requested file
	std::ifstream file(path);
	std::string line;
	while (std::getline(file, line))
	{
		if (isDynamic)
		{
			// Replace dynamic parts of dynamic file
			ReplaceInLine(line, description);
		}

		// Manually add newline character at the end of every new line since they are not read from the file
		line += '\n';

		// Send requested as part of the body
		send(connection, line.c_str(), line.size(), 0);
	}

	// Close filestream
	file.close();
}

void ParseLoginJson(const std::string& msg, std::string& username, std::string& password)
{
	// Find indexes in JSON for start and end of username
	const size_t userIndex = msg.find("\"user\"");
	const size_t userIndexStart = msg.find('"', userIndex + 6);
	const size_t userIndexEnd = msg.find('"', userIndexStart + 1);
	username = msg.substr(userIndexStart + 1, userIndexEnd - userIndexStart - 1);

	// Find indexes in JSON for start and end of password
	const size_t passwordIndex = msg.find("\"pwd\"", userIndexEnd + 1);
	const size_t passwordIndexStart = msg.find('"', passwordIndex + 5);
	const size_t passwordIndexEnd = msg.find('"', passwordIndexStart + 1);
	password = msg.substr(passwordIndexStart + 1, passwordIndexEnd - passwordIndexStart - 1);
}

void HandlePOST(const SOCKET connection, const std::string& msg, const std::string& url)
{
	// Parse username and password from json
	std::string username, password;
	ParseLoginJson(msg, username, password);

	if (url == "/login")
	{
		// Send header for response
		const std::string response = "HTTP/1.1 201 OK\nContent-Type: application/json\nContent - Length: 22\r\n\r\n";
		send(connection, response.c_str(), response.size(), 0);

		// Check if credentials are valid
		if (username == "Karl" && password == "medC")
		{
			send(connection, "{\"status\": \"Accepted\"}", 22, 0);
		}
		else
		{
			send(connection, "{\"status\": \"Rejected\"}", 22, 0);
		}
	}
	else if (url == "/signup")
	{

	}
}

void HandleRequest(const SOCKET connection)
{
	// The HTTP header needs to be read first, before we know the length of the body - if there is a body
	// Create buffer for reading from the socket & a string to save header in
	char buff[READSIZE];
	std::string req = "";

	// overRead keeps track of how many bytes into a potential HTTP body we have read
	int overRead = 0;

	// Read first section of bytes from the windows socket
	int bytes = recv(connection, buff, READSIZE, 0);
	while (bytes > 0)
	{
		// Send buffer to std::string
		const std::string sbuff = buff;

		// Check for CRLF
		const size_t CRLFIndex = sbuff.find("\r\n\r\n");
		if (CRLFIndex != std::string::npos)
		{
			req += sbuff.substr(0, CRLFIndex + 3);
			overRead = READSIZE - CRLFIndex - 4;
			break;
		}

		// No CRLF found, save entire buffer into header
		req += sbuff;

		// Read next section of bytes
		bytes = recv(connection, buff, READSIZE, 0);
	}

	// Now that the header has been read, we need to parse it
	std::stringstream reqss(req);
	std::string segment;
	std::vector<std::string> header;

	// Save important information from header
	int contentLength = 0;
	std::string username;

	// Go through every line in the header
	while (!std::getline(reqss, segment, '\n').eof())
	{
		// Save the header where every element in the vector is a new line
		header.push_back(segment);

		// Check if we find "Content-Length" in the header
		const size_t contentLengthIndex = segment.find("Content-Length:");
		if (contentLengthIndex != std::string::npos)
		{
			// Save Content-Length
			contentLength = std::stoi(segment.substr(contentLengthIndex + 15));
		}

		// Check if we find "Cookie" in the header
		const size_t cookieIndex = segment.find("Cookie:");
		if (cookieIndex != std::string::npos)
		{
			const std::string cookie = segment.substr(cookieIndex + 7);
			const size_t usernameIndex = cookie.find("username=") + 9;
			username = cookie.substr(usernameIndex);
		}
	}

	if (header.size() < 2)
	{
		std::cout << "Ignoring bad http request\n";
		return;
	}

	// Find indexes for type of request, the url requested and the http version
	const size_t typeIndex = header[0].find(' ');
	const size_t urlIndex = header[0].find(' ', typeIndex + 1);

	const std::string type = header[0].substr(0, typeIndex);
	const std::string url = header[0].substr(typeIndex + 1, urlIndex - typeIndex - 1);
	if (header[0].substr(urlIndex + 1, header[0].size() - urlIndex - 2) != "HTTP/1.1")
	{
		std::cout << "Ignoring request. Expected HTTP/1.1\n";
		closesocket(connection);
		return;
	}

	std::cout << type << ':' << url << "\n";

	if (type == "GET")
	{
		HandleGET(connection, url);
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

		HandlePOST(connection, body, url);
	}

	// Ignore everything else
	closesocket(connection);
}
