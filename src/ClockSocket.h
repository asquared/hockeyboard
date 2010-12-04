#ifndef _clocksocket_h_
#define _clocksocket_h_

#include "UDPSocket.h"

class ClockSocket : public UDPSocket {
public:
	ClockSocket( );
	~ClockSocket( );
	void send(void *thing_to_send, size_t size);

protected:
	struct sockaddr_in destination;
};

#endif
