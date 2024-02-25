#include "pch.h"

#include "socket.h"

int main()
{
	Socket server = Socket("127.0.0.1", 80);
	server.Listen(5);

	while (true)
	{
		SOCKET connection = server.Accept();

		char buff[1024];
		int bytes = recv(connection, buff, 1024, 0);
		if (bytes > 3)
		{
			std::cout << "recv length: " << bytes << '\n';

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
				std::string path = "website" + http[1];
				if (path[path.size() - 1] == '/')
				{
					path += "index.html";
				}

				send(connection, "HTTP/1.1 200 OK\r\n\r\n", 19, 0);

				std::ifstream file(path);
				std::string line;
				while (std::getline(file, line))
				{
					std::cout << line;
					send(connection, line.c_str(), line.size(), 0);
				}
				file.close();
			}
		}

		closesocket(connection);
	}
}
