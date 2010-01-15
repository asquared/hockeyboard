#include "HockeyData.h"
#define snprintf _snprintf   // stupid Microsoft

// text functions

std::string int2str(int i) {
	char c[20];
	snprintf(c, 20, "%d", i);
	return std::string(c);
}

std::string getclock(int min, int sec, int tenths) {
	char c[20];
	if (min == 0) {
		sprintf(c, ":%02d.%01d", sec, tenths);
	}
	else {
		sprintf(c, "%d:%02d", min, sec);
	}
	return std::string(c);
}


//inline void normalize_queue(PenaltyTimer& t);

HockeyData::HockeyData() {
	//name[0] = lname[0] = fname[0] = "";
	//name[1] = lname[1] = fname[1] = "";
	tm[0].sc = tm[1].sc = 0;
	tm[0].sog = tm[1].sog = 0;
	period = 1;
	PERLEN = 20 * 60 * 1000;
	INTLEN = 12 * 60 * 1000;
	otlen = 5 * 60 * 1000;
	clock.setmode(Mclock::DOWN);
	clock.set(PERLEN);
	clock_last_stopped = clock.read();
	int_clock.setmode(Mclock::DOWN);
	int_clock.set(INTLEN);
	active_clock = &clock;
	red_flash_clock.setmode(Mclock::DOWN);
	red_flash_clock.set(0);
	red_flash_team = 0;
	FLASH_LEN = 4000;
	FLASH_CYC = 250;
	FLASH_OFF = 125;
	yellow_v = false;
	yellow_h = false;
	keymode = 'l';
	sync = false;
	sstat = ' ';
	start_delay = 0;
	stop_delay = 0;
	allow_quit = false;

	roster_file = "roster.ini";
	rl = new RosterList(roster_file);

	// debug
	//Roster* b = rl->get_roster(string(""));
	//Roster* r = rl->get_roster(string("RPI"));
    //cout << rl->get_string(b, 99) << rl->get_string(r, 35) << endl;

	for ( int q = 0; q < 2; ++q ) {
		pq[q].time[0] = pq[q].time[1] = 0;
		pq[q].rem_m[0] = pq[q].rem_m[1] = 0;
		pq[q].rem_s[0] = pq[q].rem_s[1] = 0;
		for (int t = 0; t < MAX_QUEUE; ++t) pq[q].qm[t] = 0;
		pq[q].qt[0] = pq[q].qt[1] = 0;
	}
	pt_low_index = 0;
}

HockeyData::~HockeyData() {
	if (rl) delete rl;
}

void HockeyData::reloadRosters() {
	if (rl) delete rl;
	rl = new RosterList(roster_file);
}


/*
inline void normalize_queue(PenaltyTimer& t) {
	while ( t.queue_min[0] == 0 && t.queue_min[1] != 0 ) {
		for (int q = 0; q < MAX_QUEUE - 1; ++q) {
			t.queue_min[q] = t.queue_min[q+1];
		}
		t.queue_min[MAX_QUEUE-1] = 0;
		//t.queue_min[0] = t.queue_min[1];
		//t.queue_min[1] = t.queue_min[2];
		//t.queue_min[2] = t.queue_min[3];
		//t.queue_min[3] = 0;
	}
}
*/



std::string HockeyData::getStringPeriod() {
	switch (period) {
		case 0: 
			return "";
			break;
		case 1:
			return "1";
			break;
		case 2:
			return "2";
			break;
		case 3:
			return "3";
			break;
		case 4:
			return "OT";
			break;
		case 127:
			return "FINAL";
			break;
		default:
			char c[2];
			c[0] = period + 45;
			c[1] = '\0';
			std::string perchar(c);
			return std::string("OT").append(perchar);
	}
}

bool HockeyData::getRedFlash() {
	return (red_flash_clock.read() % FLASH_CYC >= FLASH_OFF);
}

void HockeyData::startRedFlash(unsigned char team) {
	if (red_flash_clock.read() == 0) {
		++tm[team].sc;
		red_flash_team = team;
		red_flash_clock.set(FLASH_LEN);
		red_flash_clock.start();
	}
}

void HockeyData::stopRedFlash(unsigned char team) {
	if (red_flash_team == team) red_flash_clock.set(0);
}

void HockeyData::setppyellow() {
	unsigned short vis, home, rmin, rsec;
	getPenaltyInfo(vis, home, rmin, rsec);

	//if ( vis-home > 0 ) yellow_v = true;
	//else yellow_v = false;

	//if ( home-vis > 0 ) yellow_h = true;
	//else yellow_h = false;
}

void HockeyData::do_sync() {
	if (!ssync.get_port()) {
		sstat = ' ';
		return;
	}
	string serialdata;
	unsigned char c,n;
	if (sync) {
		ssync.read_last(serialdata,4);
		if (serialdata.size() > 0) {
			for (int i = (int) serialdata.size()-1; i >= 0; --i) {
				c = serialdata[i];
				c ^= 0x2b;
				n = ssync.count_bits(c);
				if (n <= 1) {
					if (!clock.running()) {
						clock.adjust(-start_delay);
						clock.start();
					}
					sstat = '0';
					break;
				}
				else if (n >= 7) {
					if (clock.running()) {
						clock.stop();
						clock.adjust(stop_delay);
					}
					sstat = '1';
					break;
				}
				else {
					sstat = ' ';
				}
			}
		}
		else {
			sstat = ' ';
		}
	}
	else {
		ssync.read_last(serialdata,4);
	}
}