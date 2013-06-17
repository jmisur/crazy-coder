#include "Network.h"
#include <stdio.h>

void shutdownClient(SOCKET &clientSocket);
extern bool debug;

SOCKET startupClient(unsigned short port, const char* serverName) 
{
	#if defined (_WIN32)
		WSAData wsaData;

		if (WSAStartup(MAKEWORD(2, 2), &wsaData) == SOCKET_ERROR) 
		{
			fprintf(stderr, "\nCould Not Start Up Winsock!");
			return -1;
		}
	#endif	

	SOCKET mySocket = socket(AF_INET, SOCK_STREAM, 0);

	if (mySocket == SOCKET_ERROR) 
	{
		fprintf(stderr, "\nError opening socket");
		shutdownClient(mySocket);
		return -1;
	}

	if(debug) printf("\nClient Started");

	return mySocket;
}

void shutdownClient(SOCKET &clientSocket) 
{
	#if defined (_WIN32)
		closesocket(clientSocket);
		WSACleanup();
	#elif defined (__unix__)
		shutdown(clientSocket, SHUT_RDWR);
	#endif	

	if (debug) printf("\nClient Shutdown");
}
