#ifndef ___CLIENT___
#define ___CLIENT___
#pragma once


#include <winsock2.h>
#include <ws2tcpip.h>
#include <strsafe.h>
#include <sstream>
#include <iostream>
using std::cout;
using std::endl;
using std::cerr;
#include <String>
using std::string;
using std::wstring;
#include <vector>
using std::vector;

#pragma comment(lib, "Ws2_32.lib")

#define DEFAULT_PORT "6666"
#define BUFFER_SIZE 1024

class CUdpClient
{
public:
	CUdpClient(void);
	~CUdpClient(void);
	bool initialize();
	bool isInitialized();
	bool getGlovesReadings(float values[], const int size);
	void checkForErrors(); //prints to std::cerr on release and on MessageBox on Debug
private:
	void parseError(int err); //converts the error codes to string

	//members
	WSADATA wsaData;
	struct addrinfo *result, *ptr;
	struct addrinfo	hints;
	SOCKET connectSocket;
#ifdef UNICODE
	std::wstring error;
#else
	std::string error;
#endif
	int BufLen;
	char Buffer[BUFFER_SIZE];
	bool initialized;
};
#endif

