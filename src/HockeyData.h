#ifndef _hockeydata_h_
#define _hockeydata_h_

#include "FreeType.h"
#include "HockeyRoster.h"
#include "Mclock.h"
#include "serialsync.h"
#include <string>
#include <sstream>

// default period length is 20 min
const int PERLEN = 20 * 60 * 1000;
const int FINAL = 0x7f;

// intermission length is 12 min
const int INTLEN = 12 * 60 * 1000;

// penalty timer queue length
const int MAX_QUEUE = 8;

// text translation functions
string int2str(int i);
string getclock(int min, int sec, int tenths);

// 'time' is time since start of 1st period at which the penalty ends.
// queue is all zeros if fully inactive.
struct PenaltyTimer {
	int time;
	short queue_min[MAX_QUEUE];
	unsigned short rem_m;
	unsigned short rem_s;
	bool lowrem;

	bool active() { 
		if ( queue_min[0] > 0 || queue_min[0] < 0 && queue_min[1] > 0 ) return true;
		else return false;
	}
	int count() {
		int out = 0;
		for (int q = 0; q < MAX_QUEUE; ++q) if ( queue_min[q] > 0 ) ++out;
		return out;
	}
	int queued_time() {
		int out = 0;
		for (int q = 1; q < MAX_QUEUE; ++q) out += 60000*queue_min[q];
		return out;
		//return 60*1000*(queue_min[1] + queue_min[2] + queue_min[3]);
	}
};

class HockeyData {

public:
	string name[2];
	string lname[2];
	string fname[2];
	unsigned char sc[2];
	unsigned char period;
	int otlen;						// overtime length (5 or 20 min, but in ms)
	int clock_last_stopped;
	Mclock clock;
	Mclock int_clock;
	Mclock* active_clock;
	Mclock red_flash_clock;			// used to control red flash effect
	unsigned char red_flash_team;
	bool yellow_v, yellow_h;
	unsigned char keymode;

	// sync stuff
	SerialSync ssync;
	char sstat;
	bool sync;
	int start_delay;
	int stop_delay;
	
	bool allow_quit;

	RosterList* rl;
	string rs[2];					// names of rosters used for visiting & home teams

	PenaltyTimer pt[4];
	
	void reloadRosters();
	void addPenalty(unsigned char team, unsigned char beginperiod, int begintime, int min);
	void addPenaltyToSlot(unsigned char index, unsigned char beginperiod, int begintime, int min);
	void delPenalty(unsigned int index);
	void delFirstPenalty(unsigned int index);
	void delLastPenalty(unsigned int index);
	void editQueue(unsigned int index, std::string qstr);
	void delPenaltyAuto();
	void updatePenalty();
	void getPenaltyInfo(unsigned short& vis, unsigned short& home, unsigned short& rmin, unsigned short& rsec);
	void printPenalties(std::string* disp, freetype::font_data* base);
	string getStringPeriod();
	bool getRedFlash();
	void startRedFlash(unsigned char team);
	void stopRedFlash(unsigned char team);
	void setppyellow();
	void do_sync();

	HockeyData();
	~HockeyData();
};

#endif