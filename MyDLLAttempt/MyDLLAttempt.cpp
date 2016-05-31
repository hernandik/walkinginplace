// MyDLLAttempt.cpp : Defines the exported functions for the DLL application.

#include "stdafx.h"
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


#define BUFFER_SIZE 1024
#define DEFAULT_PORT "6666"

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


extern "C"
{
	__declspec(dllexport) int fcinco(){return 5;}
	__declspec(dllexport) void parseError(int err)
{
    // Retrieve the system error message for the last-error code
	LPTSTR lpszFunction=TEXT("Socket Error: ");
    LPVOID lpMsgBuf;
    LPVOID lpDisplayBuf;

    FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | 
        FORMAT_MESSAGE_FROM_SYSTEM |
        FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,
        err,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPTSTR) &lpMsgBuf,
        0, NULL );

    // Display the error message and exit the process

    lpDisplayBuf = (LPVOID)LocalAlloc(LMEM_ZEROINIT, 
        (lstrlen((LPCTSTR)lpMsgBuf) + lstrlen((LPCTSTR)lpszFunction) + 40) * sizeof(TCHAR)); 
    StringCchPrintf((LPTSTR)lpDisplayBuf, 
        LocalSize(lpDisplayBuf) / sizeof(TCHAR),
        TEXT("%s failed with error %d: %s"), 
        lpszFunction, err, lpMsgBuf); 
    MessageBox(NULL, (LPCTSTR)lpDisplayBuf, TEXT("Error"), MB_OK); 

    LocalFree(lpMsgBuf);
    LocalFree(lpDisplayBuf);
    //ExitProcess(err); 
}
	__declspec(dllexport) void checkForErrors() //prints to std::cerr if they exist
{
	int e = WSAGetLastError();
	if(e!=0)
	{
#ifdef _DEBUG
#ifdef UNICODE
		cerr << "Socket Error: " << e << endl;
#else
		cerr << error << endl;
#endif
		parseError(e);
#endif
	}
}
	__declspec(dllexport) bool closeUdpSocket(void)
{
	closesocket(connectSocket);
	if(result!=NULL)
	{
		freeaddrinfo(result);
	}
	WSACleanup();
	initialized=false;
	return true;
}
	__declspec(dllexport) bool initializeUDPSocket(void)
{
	//initialize functions
	result=NULL;
	ptr=NULL;
	connectSocket=INVALID_SOCKET;
	BufLen=BUFFER_SIZE; 
	for(int i=0;i<BUFFER_SIZE;++i)
		Buffer[i]=0;
	initialized=false;
	int iResult;

	/***
		The WSADATA structure contains information about the Windows Sockets implementation. The MAKEWORD(2,2) 
		parameter of WSAStartup makes a request for version 2.2 of Winsock on the system ****/
	// Initialize Winsock and the use of WS2_32.dll
	iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
	if (iResult != 0) {
#ifdef _DEBUG
		cerr << "WSAStartup failed: " << iResult << endl;
		checkForErrors();
#endif
		return false;
	}

	/**** 
		For this application, the Internet address family is unspecified so that either an IPv6 or IPv4 address can be returned. 
		The application requests the socket type to be a datagram and connectionless socket for the UDP protocol. 
	*****/
	ZeroMemory( &hints, sizeof(hints) );
	hints.ai_family = AF_UNSPEC;		// If the client application wants to connect using only IPv6 or IPv4, then the address family needs 				
	hints.ai_socktype = SOCK_DGRAM;		// to be set to AF_INET6 for IPv6 or AF_INET for IPv4 in the hints parameter. 
	hints.ai_protocol = IPPROTO_UDP;

	// Resolve the server address and port to local host and 
	iResult = getaddrinfo("127.0.0.1", DEFAULT_PORT, &hints, &result); // the local host is supposed be used in the UDK application
	if (iResult != 0) {
#ifdef _DEBUG
		cerr << "getaddrinfo failed: " << iResult << endl;
		checkForErrors();
#endif
		WSACleanup();
		return false;
	}

	//connectSocket was already initialized
	// Attempt to connect to the first address returned by
	// the call to getaddrinfo
	ptr=result;

	// Create a SOCKET for connecting to server
	connectSocket = socket(ptr->ai_family, ptr->ai_socktype, 
		ptr->ai_protocol);
	/** Calling the socket function and returning its value to the connectSocket variable. 
		For this application, use the first IP address returned by the call to getaddrinfo 
		that matched the address family, socket type, and protocol specified in the hints parameter. 
		The address family was left unspecified (AF_UNSPEC), so the returned 
		IP address could be either an IPv6 or IPv4 address for the server.
	**/

	//check if the socket was created successfully
	if (connectSocket == INVALID_SOCKET) 
	{
		checkForErrors();
		freeaddrinfo(result);
		WSACleanup();
		return false;
	}

	//Binding the socket
	iResult = bind(connectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
	if (iResult == SOCKET_ERROR) {
#ifdef _DEBUG
		checkForErrors();
#endif
		closesocket(connectSocket);
		freeaddrinfo(result);
#ifdef _DEBUG
		checkForErrors();
#endif
		connectSocket = INVALID_SOCKET;
		WSACleanup();
		return false;
	}

	// Should really try the next address returned by getaddrinfo
	// if the connect call failed
	// But for this simple example we just free the resources
	// returned by getaddrinfo and print an error message
	if (connectSocket == INVALID_SOCKET) 
	{
#ifdef _DEBUG
		cerr << "Unable to connect to server!\n" << endl;
		checkForErrors();
#endif
		closesocket(connectSocket);
		freeaddrinfo(result);
		checkForErrors();
		WSACleanup();
		return false;
	}

	//no errors
	initialized=true;
	return initialized;
}
	__declspec(dllexport) bool getGlovesReadings(float values[], const int size)
{
	int iResult = 0;

	if(initialized)
	{
		//reset buffer
		for(int j=0;j<1024;++j)
			Buffer[j]=0;

		//receive data
		iResult = recv(connectSocket, Buffer, BufLen, 0);

		//check for socket errors
		if(iResult== SOCKET_ERROR)
		{
#ifdef _DEBUG
			cerr << "recv returned an error." <<endl;
			checkForErrors();
#endif
			return false;
		}
		else //no errors, receive data
		{
			//Prints the buffer for debug purposes
			//cout << Buffer << endl;
			//vector<double> data;
			double val;
			string stream(Buffer);
			string substring;
			size_t slotPosition=0;
			size_t init=0;
			size_t pos=stream.find(',', iResult);
			//check for commas in the file
			if(pos !=string::npos)
			{
#ifdef _DEBUG
				//if there are no commas return false
				cerr << "There are no commas in the CSV string, this is not expected." << endl;
				checkForErrors();
#endif
				return false;
			}
			pos=0;

			//extract numbers from datagram
			while(init < stream.size())
			{
				double number=0;
				size_t pos = stream.find(',', init);
				//check if there is a comma in the last char

				substring = stream.substr(init, pos-init);
				if(pos !=string::npos)
				{
					std::istringstream tempstr(substring);
					if (tempstr >> val)
					{
						// ... Success!!  test is a number.
						//data.push_back(val);
						values[slotPosition]=(float)val;
#ifdef _DEBUG
						cout << values[slotPosition] << endl;
#endif
						init=pos+1; //update string init position
						++slotPosition; //updating flot slot position
					}
					else
					{
						// ... Failure!!  test is not a number
#ifdef _DEBUG
						cerr << "The substring is not a double value." << endl;
						checkForErrors();
#endif
						return false;
					}
				}
				else // the last number does not has a comma
				{
					pos=stream.size()-1;
					substring = stream.substr(init, pos-init+1);
					std::istringstream tempstr(substring);
					if (tempstr >> val)
					{
						// ... Success!!  test is a number.
						//data.push_back(val);
						values[slotPosition]=(float)val;
						init=stream.size();
						cout << values[slotPosition] << endl;
					}
					else
					{
						// ... Failure!!  test is not a number.
#ifdef _DEBUG
						cerr << "The last sub-string is not a double value." << endl;
						checkForErrors();
#endif
						return false;
					}
				}
			}
		}
	}
	else
	{
		cerr << "The socket was not initialized" << endl;
		return false;
	}

	//ok, copied all the numbers
	return true;
}
};
