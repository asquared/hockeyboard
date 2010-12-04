#ifndef _udpsocket_h_
#define _udpsocket_h_

#ifdef WINDOWS
#include <winsock.h>
#else
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#endif


class UDPSocket {
    public:
        UDPSocket( );
        virtual ~UDPSocket( );
    
    protected:
        int socket_fd;
#ifdef WINDOWS
        static int winsock_started;
#endif
};

#endif
