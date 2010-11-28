#include "ClockSocket.h"
#include <stdexcept>
#include <stdio.h>

ClockSocket::ClockSocket( ) {
#ifdef WINDOWS
	if (winsock_started == 0) {
		if (WSAStartup(0x0101, ...) != 0) {
			throw std::runtime_error("Failed to initialize Windows sockets API\n");
		}
	}

	++winsock_started;
#endif

	socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
	if (socket_fd == -1) {
		throw std::runtime_error("Failed to create UDP socket\n");
	}

	/* initialize destination address */
	destination.sin_family = AF_INET;
	destination.sin_port = htons(30003);
	inet_aton("192.168.1.100", &destination.sin_addr);

}

ClockSocket::~ClockSocket( ) {
#ifdef WINDOWS
	--winsock_started;
	if (winsock_started == 0) {
		WSACleanup( );
	}
#endif
}

void ClockSocket::send(void *thing_to_send, size_t size) {
	if (sendto(socket_fd, thing_to_send, size, 0,
			(struct sockaddr *)&destination, sizeof(destination)) == -1) {
		fprintf(stderr, "warning: failed to send out clock update");
	}
}

int ClockSocket::winsock_started = 0;
