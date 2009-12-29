#ifndef _keybuffer_h_
#define _keybuffer_h_

#include <string>

class Keybuffer {

private:
	std::string buf;
	bool enterflag;
public:
	Keybuffer();
	~Keybuffer() {}
	bool enter() { return enterflag; }
	char last() { return buf[buf.size()-1]; }
	std::string fullbuf() { return buf; }

	void add(char key);
	void clear() { buf.clear(); }

};

#endif