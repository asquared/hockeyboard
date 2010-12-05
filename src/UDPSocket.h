#ifndef _udpsocket_h_
#define _udpsocket_h_

#ifdef WINDOWS
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>

void inet_aton(const char *a, IN_ADDR *n);
void print_error(const char *msg);

/* Visual Studio apparently lacks a <stdint.h> */
typedef unsigned __int32 uint32_t;
typedef signed __int32 ssize_t;

#else
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdio.h>

#define print_error perror
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
