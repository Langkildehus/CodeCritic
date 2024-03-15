#include "pch.h"

#include "socket.h"

Socket::Socket(std::string ipstr_, int port_)
	: ipstr(ipstr_), port(port_)
{
	// Start WSA (WinSockApi)
	WSADATA wsaData;
	WORD version = MAKEWORD(2, 2);
	int wsaErr = WSAStartup(version, &wsaData);
	if (wsaErr != 0)
	{
		std::cout << "DLL NOT FOUND!\n";
		exit(-1);
	}

	// Create socket
	mSocket = INVALID_SOCKET;
	mSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (mSocket == INVALID_SOCKET)
	{
		std::cout << "Failed to create socket: " << WSAGetLastError() << "\n";
		WSACleanup();
		exit(-2);
	}

	
	// Convert ip from std::string to windows' widechar*
	std::wstring ipwstr = std::wstring(ipstr.begin(), ipstr.end());
	ip = ipwstr.c_str();


	// Bind socket to ip & port
	sockaddr_in service;
	service.sin_family = AF_INET;
	InetPton(AF_INET, ip, &service.sin_addr.s_addr);
	service.sin_port = htons(port);
	if (bind(mSocket, (SOCKADDR*)&service, sizeof(service)) == SOCKET_ERROR)
	{
		std::cout << "Bind failed: " << WSAGetLastError() << "\n";
		closesocket(mSocket);
		WSACleanup();
		exit(-3);
	}
}

Socket::~Socket()
{
	// Cleanup when destructor is called
	std::cout << "Closing socket\n";
	closesocket(mSocket);
	WSACleanup();
}

void Socket::Listen(int max)
{
	if (listen(mSocket, max) == SOCKET_ERROR)
	{
		std::cout << "Failed listening on socket: " << WSAGetLastError() << "\n";
		closesocket(mSocket);
		WSACleanup();
		exit(-4);
	}

	std::cout << "Socket listening on: " << ipstr << ":" << port << "\n";
}

SOCKET Socket::Accept()
{
	SOCKET connection = accept(mSocket, NULL, NULL);
	if (connection == INVALID_SOCKET)
	{
		std::cout << "Accept failed - ignoring connection: " << WSAGetLastError() << "\n";
		return NULL;
	}

	std::cout << "Accepted connection!\n";
	return connection;
}
