#include "serialsync.h"

// counts '1' bits in a nibble (4 bit value)
const unsigned char bitcount[] = { 0, 1, 1, 2, 1, 2, 2, 3,
								   1, 2, 2, 3, 2, 3, 3, 4 };

SerialSync::SerialSync() {
	hSerial = NULL;
	port = -1;
}

SerialSync::~SerialSync() {
	if (port >= 0) port_close();
}

int SerialSync::get_port() {
	return port;
}

// Opens the serial port.  
// Returns true if successful, false if error or already open.
bool SerialSync::port_open(int p) {
	struct termios attr;

	if (p < 0 || p > 256) return false;		// invalid port number

	// create handle
	char s[16];
	snprintf(s, 15, "/dev/ttyS%d", p);
	s[15] = 0;

    portfd = open(s, O_RDWR | O_NOCTTY);
    
    if (tcgetattr(fd, &attr) != 0) {
        perror("tcgetattr");
        port = -1;
        return false;
    }
    /* raw mode */
    cfmakeraw(&attr);
    /* 2400 baud */
    cfsetispeed(&attr, B2400);
    cfsetospeed(&attr, B2400);
    /* 8N1 default?? */
    if (tcsetattr(fd, TCSAFLUSH, &attr) != 0) {
        perror("tcsetattr");
        port = -1;
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
	for (int i = 0; i <= 31; ++i) {
		if (port_open(i)) {
			usleep(50000);
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
	if (port != -1) close(portfd);
	port = -1;
}

// Reads the last <bytes> bytes from the serial port.  
// Maximum length is 255 bytes (similar to read_all).
void SerialSync::read_last(std::string& data, int bytes) {
	char buf[256] = {0};
    int bytes_read = read(portfd, buf, 255);

    if (bytes_read < 0) {
        data = "";
    } else {
        if (bytes > bytes_read) {
            data.assign(buf, bytes_read);
        } else {
            data.assign(buf + bytes_read - bytes, bytes);
        }
    }

    /* poll() loop to clear out buffer? */
}

// Reads all the data (up to 255 bytes, which at 2400 baud is about 0.85 sec worth)
void SerialSync::read_all(std::string& data) {
	char buf[256] = {0};
    int bytes_read;

    bytes_read = read(portfd, buf, 255);
    if (bytes_read < 0) {
        perror("read");
        data = "";
    } else {
        data = buf;
    }
}

// Clears the system file buffer for the serial port.
void SerialSync::clear_buffer() {
    /* poll() loop to clear it out */
}

// Returns number of '1' bits in the given byte.  Used to decode the data.
unsigned char SerialSync::count_bits(byte in) {
	return bitcount[ (in >> 4) ] + bitcount[ (in & 0xf) ];
}

bool SerialSync::ok() {
    if (port == -1) {
        return false;
    } else {
        return true;
    }
}

