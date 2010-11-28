#ifndef _serialsync_h_
#define _serialsync_h_

#ifdef WINDOWS
#include <windows.h>
#endif

#include <string>
#include <cstdio>

typedef unsigned char byte;

class SerialSync {

private:
#ifdef WINDOWS
	HANDLE hSerial;		// Windows handle to the serial port
#else
    int portfd;
#endif
	int port;			// port number, 0 if not sets (on Unix this will become /dev/ttyS%d so zero as flag is no good - see ok())
public:
	SerialSync();
	~SerialSync();
	int get_port();
	bool port_open(int p);
	int port_scan();
	void port_close();
	void read_last(std::string& data, int bytes);
	void read_all(std::string& data);
	void clear_buffer();
	unsigned char count_bits(byte in);
    bool ok();
};

#endif
