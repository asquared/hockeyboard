#include "ClockSocket.h"
#include <stdexcept>
#include <stdio.h>

ClockSocket::ClockSocket( ) {
    char enable_loopback = 1;

#ifndef WINDOWS
    if (
        setsockopt(
            socket_fd, IPPROTO_IP, IP_MULTICAST_LOOP, 
            &enable_loopback, sizeof(enable_loopback)
        ) != 0
    ) {
        throw std::runtime_error("Failed to set multicast loopback");
    }
#endif

	/* initialize destination address */
	destination.sin_family = AF_INET;
	destination.sin_port = htons(30003);
    /* this address has been randomly generated */
	inet_aton("239.160.181.93", &destination.sin_addr);
}

ClockSocket::~ClockSocket( ) {

}

void ClockSocket::send(void *thing_to_send, size_t size) {
#ifdef WINDOWS
	/* 
	 * Dear Microsoft: if you were trying to make your interface incompatible,
	 * you couldn't have been more subtle about it by accident.
	 * (And stop indenting my code for me.
	 */
	if (sendto(socket_fd, (char *)thing_to_send, size, 0,
			(struct sockaddr *)&destination, sizeof(destination)) == -1) {
#else
	if (sendto(socket_fd, thing_to_send, size, 0,
			(struct sockaddr *)&destination, sizeof(destination)) == -1) {
#endif
		fprintf(stderr, "warning: failed to send out clock update");
	}
}

