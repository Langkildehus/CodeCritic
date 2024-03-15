#pragma once

#include "pch.h"

class Socket
{
public:
	Socket(std::string strip_, int port_);
	~Socket();

	void Listen(int max);
	SOCKET Accept();

private:
	SOCKET mSocket;
	std::string ipstr;
	LPCWSTR ip;
	int port;
};
