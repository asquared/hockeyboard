#ifndef _hockeydata_h_
#define _hockeydata_h_

#include "FreeType.h"
#include "HockeyRoster.h"
#include "Mclock.h"
#include "serialsync.h"
#include <string>
#include <sstream>

// default period length is 20 min
//const int PERLEN = 20 * 60 * 1000;
// intermission length is 12 min
//const int INTLEN = 12 * 60 * 1000;

const int FINAL = 0x7f;

// penalty timer queue length
const int MAX_QUEUE = 16;

// text translation functions
string int2str(int i);
string getclock(int min, int sec, int tenths);

// 'time' is time since start of 1st period at which the penalty ends.
// queue is all zeros if fully inactive.
/*
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
*/

class PenaltyQueue {
	friend class HockeyData;		// avoiding a huge mess

private:
	int time[2];					// time that the currently running penalty ends
	unsigned short rem_m[2];		// minutes left on upper/lower timer
	unsigned short rem_s[2];		// seconds left on upper/lower timer
	short qm[MAX_QUEUE];			// the (single) queue
	unsigned short qt[2];			// time queued on upper/lower timers

	void split_queue();									// splits queued penalties into upper/lower timers
	int pop_queue(int slot, int curr, int time_mode);	// pop penalty from queue, returns time, or zero if empty
	int push_queue(int min);							// push penalty to queue, returns position of added penalty

	bool active() {
		if (qm[0] > 0 || qm[1] > 0) return true;
		return false;
	}
	bool active(unsigned int t) {
		if (t < MAX_QUEUE && qm[t] > 0) return true;
		return false;
	}
	int count() {
		int out = 0;
		for (int q = 0; q < MAX_QUEUE; ++q) if (qm[0] > 0) ++out;
		return out;
	}
};

struct TeamData {
	string name;				// name used next to score
	string lname;				// name used on info bar area
	string fname;				// name used on (future) slideshow feature
	string rs;					// name of roster used for this team
	unsigned char sc;
	unsigned char sog;
};

class HockeyData {

public:
	//string name[2];
	//string lname[2];
	//string fname[2];
	//unsigned char sc[2];
	TeamData tm[2];
	unsigned char period;
	int PERLEN, INTLEN;				// former constants
	int otlen;						// overtime length (5 or 20 min, but in ms)
	int clock_last_stopped;
	Mclock clock;
	Mclock int_clock;
	Mclock* active_clock;
	Mclock red_flash_clock;			// used to control red flash effect
	int FLASH_LEN, 
		FLASH_CYC, 
		FLASH_OFF;					// red flash timing variables
	unsigned char red_flash_team;
	unsigned char keymode;
	bool yellow_v, yellow_h;

	// sync stuff
	SerialSync ssync;
	char sstat;
	bool sync;
	int start_delay;
	int stop_delay;
	
	bool allow_quit;

	string roster_file;
	RosterList* rl;

	//PenaltyTimer pt[4];
	PenaltyQueue pq[2];
	unsigned int pt_low_index;		// index of lowest of the 4 timers, which is displayed
	
	void reloadRosters();
	void addPenalty(unsigned char team, unsigned char beginperiod, int begintime, int min);
	//void addPenaltyToSlot(unsigned char index, unsigned char beginperiod, int begintime, int min);
	void delPenalty(unsigned int team);
	void delPenalty(unsigned int team, unsigned int slot);
	void delLastPenalty(unsigned int team);
	void setPenaltyTime(unsigned int team, unsigned int slot, int per, int time, bool start);
	void adjustPenaltyTime(unsigned int team, unsigned int slot, int time);
	void editQueue(unsigned int team, std::string qstr);
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