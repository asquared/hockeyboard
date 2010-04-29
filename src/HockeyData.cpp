#include "HockeyData.h"

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
#ifdef LAX
	perlen = 15 * 60 * 1000;
	intlen = 10 * 60 * 1000;
	otlen = 4 * 60 * 1000;
#else
	perlen = 20 * 60 * 1000;
	intlen = 12 * 60 * 1000;
	otlen = 5 * 60 * 1000;
#endif
	clock.setmode(Mclock::DOWN);
	clock.set(perlen);
	clock_last_stopped = clock.read();
	int_clock.setmode(Mclock::DOWN);
	int_clock.set(intlen);
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
	sync_tr = false;
	sstat = ' ';
	start_delay = 0;
	stop_delay = 0;
	sync_ignore_count = 2;
	c_last = 0xff;
	allow_quit = false;

	roster_file = " ";
	rl = new RosterList(roster_file);

	// debug
	//Roster* b = rl->get_roster(string(""));
	//Roster* r = rl->get_roster(string("RPI"));
    //cout << rl->get_string(b, 99) << rl->get_string(r, 35) << endl;

	for ( int q = 0; q < 2; ++q ) {
		for (int t = 0; t < ACTIVE_QUEUE; ++t) {
			pq[q].time[t] = 0;
			pq[q].rem_m[t] = 0;
			pq[q].rem_s[t] = 0;
			pq[q].qt[t] = 0;
		}
		for (int t = 0; t < MAX_QUEUE; ++t) pq[q].qm[t] = 0;
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

#ifndef LAX
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
#endif

bool HockeyData::getRedFlash() {
	return (red_flash_clock.read() % FLASH_CYC >= FLASH_OFF);
}

void HockeyData::startRedFlash(unsigned char team) {
	if (red_flash_clock.read() == 0) {
		if ( tm[team].sc < 255 ) ++tm[team].sc;
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

void HockeyData::do_sync_tr() {
	if (!ssync.get_port()) {
		sstat = ' ';
		return;
	}
	string serialdata;
	unsigned char c;
	if (sync) {
		ssync.read_all(serialdata);
		if (serialdata.size() > 0) {
			for (unsigned int i = 0; i < serialdata.size(); ++i) {
				c = serialdata[i];
				c ^= 0x2b;
				c &= 0x0f;		// no error checking yet
				if (sync_ignore_count > 0) {
					--sync_ignore_count;
					sstat = 'L';
				}
				else if (c == c_last) {
					sstat = 'N';
				}
				else {
					if (clock.read() > 60900) {
						clock.adjust(-1000);
						sync_ignore_count = 120;
					}
					else {
						clock.adjust(-100);
						sync_ignore_count = 12;
					}
					sstat = 'T';
				}
				c_last = c;
			}
		}
		else {
			sstat = ' ';
		}
	}
	else {
		ssync.read_all(serialdata);
	}
}

void HockeyData::draw_if(freetype::font_data* base) {
	// synchronizer
	if (!sync_tr) do_sync();
	else do_sync_tr();

	// clock stuff
	clock.update();
	if (!clock.running()) clock_last_stopped = clock.read();
	int_clock.update();
	red_flash_clock.update();
	int tenths, sec, min;
	get_clock_parts(clock, min, sec, tenths);
	base->qprint(740, 500, 0, "Game:   %02d:%02d.%01d", min, sec, tenths);
	if (sync) base->print(940, 460, 0, "SYNC %c", sstat);
	base->qprint(940, 500, 0, "P:%3hhu", period);
	get_clock_parts(int_clock, min, sec, tenths);
	base->qprint(740, 460, 0, "Int.:   %02d:%02d.%01d", min, sec, tenths);
	if (active_clock == &(int_clock)) base->qprint(720, 460, 0, "\x10");
	else base->qprint(720, 500, 0, "\x10");
	
	// teams and scores
	base->qprint(230, 500, 0, "SOG");
	base->qprint(110, 480, 0, "%-5s %3hhu   %3hhu", tm[0].name.substr(0,5).c_str(), tm[0].sc, tm[0].sog);
	base->qprint(110, 460, 0, "%-5s %3hhu   %3hhu", tm[1].name.substr(0,5).c_str(), tm[1].sc, tm[1].sog);
}