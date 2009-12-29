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


inline void normalize_queue(PenaltyTimer& t);

int invertTime( unsigned char period, int timeleft, int otlen ) {
	// figure out time until next change of status
	int out = (period-1) * PERLEN + ( PERLEN - timeleft );
	// overtime adjust
	if ( period > 3 && otlen < PERLEN ) out -= (period - 3) * (PERLEN - otlen);
	return out;
}

HockeyData::HockeyData() {
	name[0] = lname[0] = fname[0] = "";
	name[1] = lname[1] = fname[1] = "";
	sc[0] = 0;
	sc[1] = 0;
	period = 1;
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
	yellow_v = false;
	yellow_h = false;
	keymode = 'l';
	sync = false;
	sstat = ' ';
	start_delay = 0;
	stop_delay = 0;
	allow_quit = false;

	rl = new RosterList(string("roster.ini"));

	// debug
	//Roster* b = rl->get_roster(string(""));
	//Roster* r = rl->get_roster(string("RPI"));
    //cout << rl->get_string(b, 99) << rl->get_string(r, 35) << endl;

	for ( int i = 0; i < 4; ++i ) {
		pt[i].time = 0;
		for (int q = 0; q < MAX_QUEUE; ++q) pt[i].queue_min[q] = 0;
		//pt[i].queue_min[0] = 0;
		//pt[i].queue_min[1] = 0;
		//pt[i].queue_min[2] = 0;
		//pt[i].queue_min[3] = 0;
		pt[i].rem_m = 0;
		pt[i].rem_s = 0;
		pt[i].lowrem = false;
	}
}

HockeyData::~HockeyData() {
	delete rl;
}

void HockeyData::reloadRosters() {
	delete rl;
	rl = new RosterList(string("roster.ini"));
}

void HockeyData::addPenalty(unsigned char team, unsigned char beginperiod, int begintime, int min) {
	int curr = invertTime(beginperiod, begintime, otlen);

	unsigned char toff = team*2;

	// find index
	int index = -1;
	for (int i = toff; i < toff+2; ++i) {
		if (pt[i].queue_min[0] <= 0) {
			index = i;
			pt[i].queue_min[0] = 0;				// kill negative minutes
			break;
		}
	}
	// neither slot empty
	if ( index == -1 ) {
		if (pt[toff].rem_m < pt[toff+1].rem_m) index = toff;
		else if (pt[toff].rem_m > pt[toff+1].rem_m) index = toff+1;
		else {
			if (pt[toff].rem_s <= pt[toff+1].rem_s) index = toff;
			else index = 1;
		}
	}
	if (index == -1) return;								// if screwed up
	if (pt[index].queue_min[MAX_QUEUE-1] != 0) return;		// if queue full

	// add to queue_min
	for (int q = 0; q < MAX_QUEUE; ++q) {
		if (pt[index].queue_min[q] == 0) {
			pt[index].queue_min[q] = min;
			// double major
			if (min == 4 && q <= MAX_QUEUE-2) {
				pt[index].queue_min[q] = 2;
				pt[index].queue_min[q+1] = 2;
			}
			break;
		}
	}

	// add on any time in earlier penalties to these penalty minutes
	pt[index].time = curr + min*60*1000 + pt[index].rem_m*60*1000 + pt[index].rem_s*1000;
}

void HockeyData::addPenaltyToSlot(unsigned char index, unsigned char beginperiod, int begintime, int min) {
	int curr = invertTime(beginperiod, begintime, otlen);

	if (index < 0 && index > 3) return;						// check index
	if (pt[index].queue_min[MAX_QUEUE-1] != 0) return;		// if queue full

	// add to queue_min
	for (int q = 0; q < MAX_QUEUE; ++q) {
		if (pt[index].queue_min[q] == 0) {
			pt[index].queue_min[q] = min;
			// double major
			if (min == 4 && q <= MAX_QUEUE-2) {
				pt[index].queue_min[q] = 2;
				pt[index].queue_min[q+1] = 2;
			}
			break;
		}
	}

	// add on any time in earlier penalties to these penalty minutes
	pt[index].time = curr + min*60*1000 + pt[index].rem_m*60*1000 + pt[index].rem_s*1000;
}

void HockeyData::delPenalty(unsigned int index) {
	if (index >= 0 && index <= 3) {
		for (int q = 0; q < MAX_QUEUE; ++q) pt[index].queue_min[q] = 0;
		//pt[index].queue_min[0] = 0;
		//pt[index].queue_min[1] = 0;
		//pt[index].queue_min[2] = 0;
		//pt[index].queue_min[3] = 0;
	}
}

void HockeyData::delFirstPenalty(unsigned int index) {
	int curr = invertTime(period, clock.read(), otlen);

	short top = pt[index].queue_min[0];
	if ( top != 0 ) {
		pt[index].queue_min[0] = 0;
		if ( top < 0 ) pt[index].queue_min[1] = 0;
		normalize_queue(pt[index]);
		if (top > 0 && pt[index].queue_min[0] > 0) {
			pt[index].time = curr;
			for (unsigned char q = 0; q < 8 && pt[index].queue_min[q] != 0; ++q)
				pt[index].time += pt[index].queue_min[q]*60*1000;
		}
	}
}

void HockeyData::delLastPenalty(unsigned int index) {
	for ( unsigned char q = MAX_QUEUE - 1; q >= 0; --q ) {
		if (pt[index].queue_min[q] != 0) {
			pt[index].time -= pt[index].queue_min[q]*60*1000;
			pt[index].queue_min[q] = 0;
			break;
		}
	}
}

void HockeyData::editQueue(unsigned int index, std::string qstr) {

	// parse
	vector<string> vs;
	int nums[] = { -1, -1, -1, -1, -1, -1, -1, -1 };
	IniParser::trim(qstr);
	IniParser::remove_dup_delim(qstr, " ");
	IniParser::parsedelim(qstr, " ", vs);
	for (unsigned int i = 0; i < min(8, vs.size()); ++i) {
		nums[i] = IniParser::parseint(vs[i], -1);
	}

	//std::stringstream ss;
	//ss << qstr;
	int temp = -1;
	int old_queue_sum = 0;
	int new_queue_sum = 0;
	bool locked = false;
	for ( int i = 0; i < MAX_QUEUE; ++i ) {
		old_queue_sum += pt[index].queue_min[i];
		pt[index].queue_min[i] = 0;
		//ss >> temp;
		if ( nums[i] > 0 && nums[i] <= 5 && !locked ) 
			pt[index].queue_min[i] = nums[i];
		else 
			locked = true;
		new_queue_sum += pt[index].queue_min[i];
	}
	int curr = invertTime(period, clock.read(), otlen);
	pt[index].time += (new_queue_sum - old_queue_sum) * 60 * 1000;
}

void HockeyData::delPenaltyAuto() {
	unsigned char ac = 0; 
	unsigned char ai;
	for (int i = 0; i < 4; ++i) {
		if ( pt[i].active() ) { 
			++ac;
			ai = i;
		}
	}
	if ( ac == 1 && pt[ai].count() == 1 ) delFirstPenalty(ai);
}

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

void HockeyData::updatePenalty() {
	int curr = invertTime(period, clock.read(), otlen);
	int low = 0x7fffffff;
	int lowindex = -1;
	int rem;
	for (int i = 0; i < 4; ++i) {
		rem = 0;
		if (pt[i].queue_min[0] != 0) rem = pt[i].time - curr;			// update clock

		// kill front of queue at right time
		if (pt[i].queued_time() != 0 && rem <= pt[i].queued_time()) {
			pt[i].queue_min[0] = 0;										
			normalize_queue(pt[i]);
		}

		// kill negative time after 5 seconds
		else if (pt[i].queued_time() == 0 && rem <= -5000 ) {
			pt[i].queue_min[0] = 0;
		}

		// negative minutes (only at end of timer, when only one should be in queue)
		else if ( rem <= 0 && pt[i].queue_min[0] > 0 ) pt[i].queue_min[0] = -(pt[i].queue_min[0]);
		else if ( rem > 0 && pt[i].queue_min[0] < 0 ) pt[i].queue_min[0] = -(pt[i].queue_min[0]);

		// don't show negative time
		if (pt[i].queue_min[0] <= 0 && pt[i].queue_min[1] == 0) {
			pt[i].rem_m = 0;
			pt[i].rem_s = 0;
		}
		else { 
			if ( rem % 1000 == 0 ) rem /= 1000;
			else rem = (rem + 1000) / 1000;
			if (rem < low) { 
				low = rem;
				lowindex = i;
			}
			pt[i].rem_m = rem / 60;
			pt[i].rem_s = rem % 60;
		}
	}
	for (int i = 0; i < 4; ++i) {
		if ( i == lowindex ) pt[i].lowrem = true;
		else pt[i].lowrem = false;
	}    
}

void HockeyData::getPenaltyInfo(unsigned short& vis, unsigned short& home, unsigned short& rmin, unsigned short& rsec) {
	updatePenalty();

	vis = 5;
	home = 5;
	
	if ( pt[0].active() ) --vis;
	if ( pt[1].active() ) --vis;
	if ( pt[2].active() ) --home;
	if ( pt[3].active() ) --home;

	rmin = rsec = 0;
	for ( int i = 0; i < 4; ++i ) {
		if ( pt[i].lowrem ) {
			rmin = pt[i].rem_m;
			rsec = pt[i].rem_s;
		}
	}

}

void HockeyData::printPenalties(std::string* disp, freetype::font_data* base) {
	//std::stringstream ss;
	for (int i = 0; i < 2; ++i ) {
		base->print(360, 400.0f-20*i, 0, "%d --%2hu:%02hu", i+1, pt[i].rem_m, pt[i].rem_s );
		base->print(480, 400.0f-20*i, 0, "%hd %hd %hd %hd %hd %hd %hd %hd", pt[i].queue_min[0],
			pt[i].queue_min[1], pt[i].queue_min[2], pt[i].queue_min[3], pt[i].queue_min[4], 
			pt[i].queue_min[5], pt[i].queue_min[6], pt[i].queue_min[7]);

		//ss << (i+1) << " -- "; 
		//ss << pt[i].rem_m << ":" << pt[i].rem_s / 10 << pt[i].rem_s % 10;
		//ss << "   " << pt[i].queue_min[0] << " " << pt[i].queue_min[1];
		//ss << " " << pt[i].queue_min[2] << " " << pt[i].queue_min[3];
		//ss << " " << pt[i].queue_min[4] << " " << pt[i].queue_min[5];
		//ss << " " << pt[i].queue_min[6] << " " << pt[i].queue_min[7];
		//base->print(360, 400.0f-20*i, 0, ss.str().c_str());
		//ss.str("");
	}
	for (int i = 2; i < 4; ++i ) {
		base->print(720, 440.0f-20*i, 0, "%d --%2hu:%02hu", i+1, pt[i].rem_m, pt[i].rem_s );
		base->print(840, 440.0f-20*i, 0, "%hd %hd %hd %hd %hd %hd %hd %hd", pt[i].queue_min[0],
			pt[i].queue_min[1], pt[i].queue_min[2], pt[i].queue_min[3], pt[i].queue_min[4], 
			pt[i].queue_min[5], pt[i].queue_min[6], pt[i].queue_min[7]);

		//ss << ((i+1)%10) << " -- ";
		//ss << pt[i].rem_m << ":" << pt[i].rem_s / 10 << pt[i].rem_s % 10;
		//ss << "   " << pt[i].queue_min[0] << " " << pt[i].queue_min[1];
		//ss << " " << pt[i].queue_min[2] << " " << pt[i].queue_min[3];
		//ss << " " << pt[i].queue_min[4] << " " << pt[i].queue_min[5];
		//ss << " " << pt[i].queue_min[6] << " " << pt[i].queue_min[7];
		//base->print(720, 440.0f-20*i, 0, ss.str().c_str());
		//ss.str("");
	}
}

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

// timings for flashing
#define FLASH_LEN 4000
#define FLASH_CYC 250
#define FLASH_OFF 125

bool HockeyData::getRedFlash() {
	return (red_flash_clock.read() % FLASH_CYC >= FLASH_OFF);
}

void HockeyData::startRedFlash(unsigned char team) {
	if (red_flash_clock.read() == 0) {
		++sc[team];
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