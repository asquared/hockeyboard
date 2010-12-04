#include "UDPSocket.h"
#include <stdexcept>

UDPSocket::UDPSocket( ) {
#ifdef WINDOWS
    WSADATA wsad;
	if (winsock_started == 0) {
		if (WSAStartup(0x0101, &wsad) != 0) {
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
