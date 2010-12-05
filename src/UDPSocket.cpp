#include "UDPSocket.h"
#include <stdexcept>

#ifdef WINDOWS

void inet_aton(const char *a, IN_ADDR *n) {
	n->s_addr = inet_addr(a);	
}

void print_error(const char *msg) {
	/* Dear Microsoft: This is *so* much easier on Unix... */
	TCHAR buf[128];
	memset(buf, 0, sizeof(buf));

	FormatMessage(
		FORMAT_MESSAGE_FROM_SYSTEM | 
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		GetLastError( ),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		buf, sizeof(buf) - 1, NULL
	);

	fprintf(stderr, "%s: %s\n", msg, buf);

}

#endif

UDPSocket::UDPSocket( ) {
#ifdef WINDOWS
    WSADATA wsad;
	if (winsock_started == 0) {
		if (WSAStartup(0x0002, &wsad) != 0) {
			throw std::runtime_error("Failed to initialize Windows sockets API\n");
		}
	}

	++winsock_started;
#endif

	socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
	if (socket_fd == -1) {
		throw std::runtime_error("Failed to create UDP socket\n");
	}
}

UDPSocket::~UDPSocket( ) {
#ifdef WINDOWS
    closesocket(socket_fd);
	--winsock_started;
	if (winsock_started == 0) {
		WSACleanup( );
	}
#else
    close(socket_fd);
#endif
}

#ifdef WINDOWS
int UDPSocket::winsock_started = 0;
#endif
