#include "Mclock.h"

Mclock::Mclock() {
	mode = UP;
	currtime = 0;
	lasttime = 0;
	run = false;
}

Mclock::Mclock(int starttime) {
	mode = DOWN;
	currtime = starttime;
	lasttime = 0;
	run = false;
}

void Mclock::update() {
	if (run) {
		int temp;
		temp = (int) timeGetTime();
		int diff = temp - lasttime;
		switch (mode) {
			case UP:
				currtime += diff;
				break;
			case DOWN:
				currtime -= diff;
				if ( currtime < 0 ) {
					currtime = 0;
					run = false;
				}
				break;
		}
		lasttime = temp;
	}
}


int Mclock::read() {
	return currtime;
}


void Mclock::set(int starttime) {
	currtime = starttime;
}

void Mclock::adjust(int offset) {
	currtime += offset;
	if (currtime < 0) currtime = 0;
}


void Mclock::setmode(char newmode) {
	if (newmode < 2) mode = newmode;
	run = false;
}

void Mclock::start() {
	if (!run) {
		run = true;
		lasttime = (int) timeGetTime();
	}
}

void Mclock::stop() {
	update();
	run = false;
}

void Mclock::toggle() {
	if ( run ) stop();
	else start();
}

