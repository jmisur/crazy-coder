#ifndef _NET_H_
#define _NET_H_

#if defined(_WIN32)
	#include <winsock2.h>
#elif defined(__unix__)
	#include <sys/socket.h>
	#include <netdb.h>
	#include <stdio.h>
	#include <stdlib.h>
#else
	#error "Unix/Win32 platform only"
#endif

#include "../Shared/Defines.h"

class CPacket
{
public:
	CPacket()
	{
		for (int i = 0; i < PACKET_SIZE; i++) buf[i] = 0;
	}
	CPacket(const CPacket &p)
	{
		*this = p;
	}
	CPacket& operator=(const CPacket &p)
	{
		for (int i = 0; i < PACKET_SIZE; i++) buf[i] = p.buf[i];
		return *this;
	}
	void null()
	{
		for (int i = 0; i < PACKET_SIZE; i++) buf[i] = 0;
	}
	void Set(GAME_STATE gs, PLAYER player, PLAYER_ACTION move, PLAYER_ACTION plant)
	{
		buf[GS] = gs; buf[P] = player; buf[M] = move; buf[PL] = plant;
	}
	char buf[PACKET_SIZE];
};

#if defined (__unix__)
	typedef int SOCKET;
#endif

#endif
