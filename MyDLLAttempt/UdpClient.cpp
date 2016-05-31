#include "UdpClient.h"


CUdpClient::CUdpClient(void) : wsaData(), result(NULL), ptr(NULL), hints(), connectSocket(INVALID_SOCKET), error(), BufLen(BUFFER_SIZE), Buffer(), initialized(false)
{
}

CUdpClient::~CUdpClient(void)
{
	closesocket(connectSocket);
	if(result!=NULL)
	{
		freeaddrinfo(result);
	}
	WSACleanup();
	initialized=false;
}
bool CUdpClient::initialize(void)
{
	int iResult;
	/***
		The WSADATA structure contains information about the Windows Sockets implementation. The MAKEWORD(2,2) 
		parameter of WSAStartup makes a request for version 2.2 of Winsock on the system ****/
	// Initialize Winsock and the use of WS2_32.dll
	iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
	if (iResult != 0) {
		cerr << "WSAStartup failed: " << iResult << endl;
		checkForErrors();
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
		cerr << "getaddrinfo failed: " << iResult << endl;
		checkForErrors();
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
		return 1;
	}

	//Binding the socket
	iResult = bind(connectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
	if (iResult == SOCKET_ERROR) {
		checkForErrors();
		closesocket(connectSocket);
		freeaddrinfo(result);
		checkForErrors();
		connectSocket = INVALID_SOCKET;
		WSACleanup();
		return false;
	}

	// Should really try the next address returned by getaddrinfo
	// if the connect call failed
	// But for this simple example we just free the resources
	// returned by getaddrinfo and print an error message
	if (connectSocket == INVALID_SOCKET) {
		cerr << "Unable to connect to server!\n" << endl;
		checkForErrors();
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

void CUdpClient::parseError(int err)
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

void CUdpClient::checkForErrors() //prints to std::cerr if they exist
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

bool CUdpClient::getGlovesReadings(float values[], const int size)
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
			checkForErrors();
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
				//if there are no commas return false
				cerr << "There are no commas in the CSV string, this is not expected." << endl;
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
						cout << values[slotPosition] << endl;
						init=pos+1; //update string init position
						++slotPosition; //updating flot slot position
					}
					else
					{
						// ... Failure!!  test is not a number.
						cerr << "The substring is not a double value." << endl;
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
						cerr << "The last sub-string is not a double value." << endl;
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

