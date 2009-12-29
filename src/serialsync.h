#ifndef _serialsync_h_
#define _serialsync_h_

#include <windows.h>
#include <string>
#include <cstdio>

typedef unsigned char byte;

class SerialSync {

private:
	HANDLE hSerial;		// Windows handle to the serial port
	int port;			// port number, 0 if not sets

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

};

#endif