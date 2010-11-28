#include "serialsync.h"

// counts '1' bits in a nibble (4 bit value)
const unsigned char bitcount[] = { 0, 1, 1, 2, 1, 2, 2, 3,
								   1, 2, 2, 3, 2, 3, 3, 4 };

SerialSync::SerialSync() {
	hSerial = NULL;
	port = 0;
}

SerialSync::~SerialSync() {
	if (port) port_close();
}

int SerialSync::get_port() {
	return port;
}

// Opens the serial port.  
// Returns true if successful, false if error or already open.
bool SerialSync::port_open(int p) {
	if (port) return false;					// port already open
	if (p < 1 || p > 256) return false;		// invalid port number

	// create handle
	char s[16];
	sprintf(s, "\\\\.\\COM%d", p);
	s[15] = 0;
	hSerial = CreateFile(s,
		GENERIC_READ | GENERIC_WRITE,
		0,
		0,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		0);
	
	if ( hSerial == INVALID_HANDLE_VALUE ) {
		if ( GetLastError() == ERROR_FILE_NOT_FOUND ) {
			// serial port does not exist
			port = 0;
			return false;
		}
		// other error
		port = 0;
		return false;
	}

	// set up serial port parameters (bit rate, etc.)
	DCB dcbSerialParams = {0};
	dcbSerialParams.DCBlength = sizeof(dcbSerialParams);
	if (!GetCommState(hSerial, &dcbSerialParams)) {
		// error getting state
		port = 0;
		return false;
	}
	dcbSerialParams.BaudRate=CBR_2400;				// 2400 baud
	dcbSerialParams.ByteSize=8;						// 8 bit byte
	dcbSerialParams.StopBits=ONESTOPBIT;			// 1 stop bit
	dcbSerialParams.Parity=NOPARITY;				// no parity bit
	if(!SetCommState(hSerial, &dcbSerialParams)){
		// error setting serial port state
		port = 0;
		return false;
	}

	// set up various timeout values
	// these are mostly set to zero, to get Windows to give us a byte
	// as soon as it is received, since it is constantly getting data
	COMMTIMEOUTS timeouts = {0};
	timeouts.ReadIntervalTimeout = MAXDWORD;
	timeouts.ReadTotalTimeoutConstant = 0;
	timeouts.ReadTotalTimeoutMultiplier = 0;
	timeouts.WriteTotalTimeoutConstant = 0;
	timeouts.WriteTotalTimeoutMultiplier = 0;
	if(!SetCommTimeouts(hSerial, &timeouts)){
		// error 
		port = 0;
		return false;
	}

	port = p;
	return true;
}

// Scans for a port with any sort of data on it matching the parameters
// that we expect (bit rate, etc.)
int SerialSync::port_scan() {
	std::string data;
	port_close();
	for (int i = 1; i <= 32; ++i) {
		if (port_open(i)) {
			Sleep(50);
			read_all(data);
			if ( data.size() > 0 ) {
				port = i;
				break;
			}
			port_close();
		}
	}

	return port;
}

// Closes port.
void SerialSync::port_close() {
	if (port != 0) CloseHandle(hSerial);
	port = 0;
}

// Reads the last <bytes> bytes from the serial port.  
// Maximum length is 255 bytes (similar to read_all).
void SerialSync::read_last(std::string& data, int bytes) {
	COMSTAT lpStat;
	ClearCommError(hSerial, NULL, &lpStat);
	int buflen = lpStat.cbInQue;

	char buf[256] = {0};
	DWORD dwBytesRead = 0;
	if(!ReadFile(hSerial, buf, min(buflen,255), &dwBytesRead, NULL)){
		// error 
		data = "";
	}
	else {
		data.assign(max(buf,buf + min(buflen,255) - bytes), min(buflen,bytes));
	}

	// clear anything left in the system buffer, in case it gets backlogged
	buflen -= dwBytesRead;
	if (buflen > 0) PurgeComm(hSerial, PURGE_RXCLEAR);
	// debug
	//printf("%s\n", buf);
	printf("%s\n", data.c_str());
}

// Reads all the data (up to 255 bytes, which at 2400 baud is about 0.85 sec worth)
void SerialSync::read_all(std::string& data) {
	COMSTAT lpStat;
	ClearCommError(hSerial, NULL, &lpStat);
	int buflen = lpStat.cbInQue;

	char buf[256] = {0};
	DWORD dwBytesRead = 0;
	if(!ReadFile(hSerial, buf, min(buflen,255), &dwBytesRead, NULL)){
		// error 
		data = "";
	}
	else {
		data = buf;
	}

	// clear anything left in the system buffer, in case it gets backlogged
	buflen -= dwBytesRead;
	if (buflen > 0) PurgeComm(hSerial, PURGE_RXCLEAR);
	// debug
	printf("%s\n", data.c_str());
}

// Clears the system file buffer for the serial port.
void SerialSync::clear_buffer() {
	if (port) FlushFileBuffers(hSerial);
}

// Returns number of '1' bits in the given byte.  Used to decode the data.
unsigned char SerialSync::count_bits(byte in) {
	return bitcount[ (in >> 4) ] + bitcount[ (in & 0xf) ];
}

bool SerialSync::ok() {
    if (port == 0) { 
        return false;
    } else {
        return true;
    }
}
