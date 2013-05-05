
#include <fcntl.h>
#include "Socket.h"

Socket::Socket()
{
	sock = socket(AF_INET,SOCK_STREAM,0);
}
Socket::~Socket()
{
	close(sock);
}
void Socket::connect(std::string host, int port)
{
	struct sockaddr_in that = {0};
	that.sin_family = AF_INET;
	that.sin_port = htons(port);
	that.sin_addr.s_addr = inet_addr(host.c_str());
	#ifdef _WIN32
	DWORD nonblocking = 1;
	ioctlsocket(sock, FIONBIO, &nonblocking);
	#else
	int nonblocking = 1;
	fcntl(sock, F_SETFL, O_NONBLOCK, nonblocking);
	#endif
	::connect(sock, (struct sockaddr *) &that, sizeof(that));
}
void Socket::send(std::string data)
{
	::send(sock,data.c_str(),data.size(),0);
}
std::string Socket::recv()
{
	// TODO implement reading in a buffer ? didn't do since it would be harder to handle
	char c(0);
	std::string message("");
	while(1)
	{
		if (::recv(sock,&c,1,0) > 0)
		{
			if (c != 0)
				message += c;
			else break;
		} else break; // we won't wait for a message if it hasn't been sent yet
	}
	return message;
}

// Socket init and quit are only needed on win32

void Socket::init()
{
	#ifdef WIN32
	WSADATA WsaData;
	WSAStartup(MAKEWORD(2,2), &WsaData);
	#endif
}

void Socket::quit()
{
	#ifdef _WIN32
	WSACleanup();
	#endif
}

