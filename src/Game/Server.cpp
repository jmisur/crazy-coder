#include "Network.h"
#include <stdio.h>

void shutdownServer(SOCKET &socket);
extern bool debug;

SOCKET startupServerForListening(unsigned short port) 
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
		fprintf(stderr, "\nError Opening Socket!");
		return -1;
	}

	struct sockaddr_in server;

	server.sin_family = AF_INET;
	server.sin_port = htons(port);
	server.sin_addr.s_addr = INADDR_ANY;
  
	if (bind(mySocket, (sockaddr*)&server, sizeof(server)) == SOCKET_ERROR) 
	{
		fprintf(stderr, "\nBind Failed!");
		shutdownServer(mySocket);
		return -2;
	}

	if (listen(mySocket, 5) == SOCKET_ERROR) 
	{
		fprintf(stderr, "\nListen Failed!");
		shutdownServer(mySocket);
		return -3;
	}

	if (debug) printf("\nServer Started.");

	return mySocket;
}

void shutdownServer(SOCKET &socket) 
{
	#if defined (_WIN32)
		closesocket(socket);
		WSACleanup();
	#elif defined (__unix__)
		shutdown(socket, SHUT_RDWR);
	#endif	

	if(debug) printf("\nServer Shutdown.");
}
