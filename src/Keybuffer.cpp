#include "Keybuffer.h"

Keybuffer::Keybuffer() {
	enterflag = false;
}

void Keybuffer::add(char key) {
	enterflag = false;
	if ( key == 8 && buf.size() > 0 ) {
		buf.resize(buf.size() - 1, 0);
	}
	else if ( key == '\n' || key == '\r' ) {
		enterflag = true;
	}
	else if ( key >= 32 && key < 127 ) {
		buf.push_back(key);
	}
}