#include "SyncSocket.h"

#include <string.h>
#include <stdexcept>

SyncSocket::SyncSocket( ) {
    /* bind to multicast receive address */
    struct sockaddr_in listen_addr;
    struct ip_mreq mreq;

    inet_aton("239.160.181.93", &mreq.imr_multiaddr);
#ifdef WINDOWS
	mreq.imr_interface.s_addr = INADDR_ANY;
#else
	inet_aton("0.0.0.0", &mreq.imr_interface);
#endif
    memset(&listen_addr, 0, sizeof(listen_addr));
    listen_addr.sin_family = AF_INET;
    listen_addr.sin_port = htons(30004);

    if (bind(socket_fd, (struct sockaddr *)&listen_addr, sizeof(listen_addr)) == -1) {
		print_error("bind");
        throw std::runtime_error("failed to bind to port 30004");        
    }

#ifdef WINDOWS
	/* Dear Microsoft: This is *exactly* what "void *" pointers were meant for! */
	if (setsockopt(socket_fd, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char *)&mreq, sizeof(mreq)) != 0) {	
#else
	if (setsockopt(socket_fd, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq, sizeof(mreq)) != 0) {
#endif
		print_error("setsockopt");
        throw std::runtime_error("failed to bind to multicast address 239.160.181.93");        
    }
}

SyncSocket::~SyncSocket( ) {

}

bool SyncSocket::can_receive( ) {
    fd_set fds;
    int ret;
    struct timeval tv0;

    memset(&tv0, 0, sizeof(struct timeval));

    FD_ZERO(&fds);
    FD_SET(socket_fd, &fds);

    ret = select(socket_fd + 1, &fds, NULL, NULL, &tv0);

    if (ret > 0) {
        return true;
    } else if (ret < 0) {
        throw std::runtime_error("select( ) failed");
    } else {
        return false;
    }
}

size_t SyncSocket::receive(void *buf, size_t size) {
#ifdef WINDOWS
	/*
	 * Dear Microsoft: RTFMan Pages on Sockets. 
	 * Hint: Look for "void *". It's everywhere. 
	 */
	ssize_t ret = recvfrom(socket_fd, (char *)buf, size, 0, NULL, NULL);
#else
    ssize_t ret = recvfrom(socket_fd, buf, size, 0, NULL, NULL);
#endif
    if (ret < 0) {
        throw std::runtime_error("recvfrom( ) failed");
    } else {
        return (size_t) ret;
    }
}
