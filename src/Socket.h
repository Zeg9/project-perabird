#ifndef __SOCKET_H__
#define __SOCKET_H__

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0501
#endif
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
typedef SOCKET socket_t;
typedef int socklen_t;
#define close closesocket
#else
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <netdb.h>
#include <unistd.h>
#include <arpa/inet.h>
typedef int socket_t;
#endif

#include <string>
#include <queue>

class Socket {
	public:
		Socket();
		~Socket();
		void connect(std::string host, int port);
		void send(std::string data);
		std::string recv();
		static void init();
		static void quit();
	private:
		socket_t sock;
		std::queue<std::string> queuedMessages;
};

#endif /* __SOCKET_H__ */

