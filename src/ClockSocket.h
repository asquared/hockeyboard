#ifndef _clocksocket_h_
#define _clocksocket_h_

#ifdef WINDOWS
#include <winsock.h>
#else
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#endif

class ClockSocket {
public:
	ClockSocket( );
	~ClockSocket( );
	void send(void *thing_to_send, size_t size);

protected:
	int socket_fd;
	static int winsock_started;

	struct sockaddr_in destination;
};

#endif
