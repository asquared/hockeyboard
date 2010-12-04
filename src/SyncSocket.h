#ifndef _syncsocket_h_
#define _syncsocket_h_

#include "UDPSocket.h"

class SyncSocket : public UDPSocket {
    public:
        SyncSocket( );
        ~SyncSocket( );

        bool can_receive( );
        size_t receive(void *buf, size_t size);
};

#endif
