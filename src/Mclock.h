// Count-up or count-down clock using timeGetTime function (multimedia timer) of Windows.
// Less precise than Qclock, but hopefully more reliable and just as accurate.
#ifndef _mclock_h_
#define _mclock_h_

#ifdef _MSC_VER
#pragma comment(lib, "winmm")
#endif //_MSC_VER

#include <windows.h>

class Mclock {

private:
	int currtime;
	int lasttime;
	char mode;
	bool run;
public:
	Mclock();
	Mclock(int starttime);

	const static char UP = 0;
	const static char DOWN = 1;


	void update();
	int read();
	void set(int starttime);
	void adjust(int offset);
	char getmode() {return mode;}
	void setmode(char newmode);
	bool running() {return run;}

	void start();
	void stop();
	void toggle();
	

};

#endif