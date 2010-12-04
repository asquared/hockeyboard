#include "ClockSocket.h"
#include <stdexcept>
#include <stdio.h>

ClockSocket::ClockSocket( ) {
    char enable_loopback = 1;

    if (
        setsockopt(
            socket_fd, IPPROTO_IP, IP_MULTICAST_LOOP, 
            &enable_loopback, sizeof(enable_loopback)
        ) != 0
    ) {
        throw std::runtime_error("Failed to set multicast loopback");
    }

	/* initialize destination address */
	destination.sin_family = AF_INET;
	destination.sin_port = htons(30003);
    /* this address has been randomly generated */
	inet_aton("239.160.181.93", &destination.sin_addr);
}

ClockSocket::~ClockSocket( ) {

}

void ClockSocket::send(void *thing_to_send, size_t size) {
	if (sendto(socket_fd, thing_to_send, size, 0,
			(struct sockaddr *)&destination, sizeof(destination)) == -1) {
		fprintf(stderr, "warning: failed to send out clock update");
	}
}

