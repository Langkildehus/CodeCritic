#pragma once

#include "pch.h"

class Socket
{
public:
	Socket(const std::string strip_, const int port_);
	~Socket();

	void Listen(const int max);
	SOCKET Accept() const;

private:
	SOCKET mSocket;
	LPCWSTR ip;
	const std::string ipstr;
	const int port;
};
