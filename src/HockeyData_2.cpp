#include "HockeyData.h"

int invertTime( unsigned char period, int timeleft, int PERLEN, int otlen ) {
	// figure out time until next change of status
	int out = (period-1) * PERLEN + ( PERLEN - timeleft );
	// overtime adjust
	if ( period > 3 && otlen < PERLEN ) out -= (period - 3) * (PERLEN - otlen);
	return out;
}

void PenaltyQueue::split_queue() {
	int inc_time[2];
	inc_time[0] = time[0];
	inc_time[1] = time[1];
	qt[0] = qt[1] = 0;
	for (unsigned int q = 2; q < MAX_QUEUE; ++q) {
		if (!qm[q]) return;  // end of queue
		if (inc_time[0] <= inc_time[1]) {
			qt[0] += qm[q];
			inc_time[0] += qm[q] * 60000;
		}
		else {
			qt[1] += qm[q];
			inc_time[1] += qm[q] * 60000;
		}
	}
}

// time_mode:
// 0: start after previous penalty (previous penalty expired normally)
// 1: start now (previous penalty deleted)
int PenaltyQueue::pop_queue(int slot, int curr, int time_mode) {
	qm[slot] = qm[2];
	if (!time_mode) time[slot] += qm[slot] * 60000;
	else time[slot] = curr + qm[slot] * 60000;
	for (int q = 3; q < MAX_QUEUE; ++q) {
		qm[q-1] = qm[q];
		if (!qm[q]) break;
	}
	qm[MAX_QUEUE-1] = 0;
	split_queue();
	return qm[slot];
}

int PenaltyQueue::push_queue(int min) {
	for (int q = 0; q < MAX_QUEUE; ++q) {
		if (qm[q] <= 0) { 
			qm[q] = min;
			split_queue();
			return q;
		}
	}
	return MAX_QUEUE;	// queue full
}

void HockeyData::addPenalty(unsigned char team, unsigned char beginperiod, int begintime, int min) {
	int curr = invertTime(beginperiod, begintime, PERLEN, otlen);
	unsigned int slot = pq[team].push_queue(min);
	if ( slot < 2 ) {
		pq[team].time[slot] = curr + min * 60000;
	}
}

/*
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
*/

// Delete entire team's penalties
void HockeyData::delPenalty(unsigned int team) {
	if (team >= 0 && team <= 1) {
		for (int t = 0; t < MAX_QUEUE; ++t) pq[team].qm[t] = 0;
	}
}

// Delete single penalty
void HockeyData::delPenalty(unsigned int team, unsigned int slot) {
	if (team >= 2 || slot >= MAX_QUEUE) return;
	int curr = invertTime(period, clock.read(), PERLEN, otlen);
	bool partial = false;

	if (pq[team].qm[slot] >= 4 && !(pq[team].qm[slot] & 1)) {
		pq[team].qm[slot] -= 2;
		partial = true;
	}
	else if (pq[team].qm[slot] >= 7 && (pq[team].qm[slot] & 1)) {
		pq[team].qm[slot] -= 5;
		partial = true;
	}

	if (slot == 0 || slot == 1) {
		if (partial) {
			if (pq[team].time[slot] - curr > pq[team].qm[slot] * 60000) 
				pq[team].time[slot] = curr + pq[team].qm[slot] * 60000;
		}
		else {
			pq[team].pop_queue(slot, curr, 1);
			//pq[team].time[slot] = curr + pq[team].qm[slot] * 60000;
			//pq[team].split_queue();
		}
	}
	else {
		if (!partial) {
			pq[team].qm[slot] = 0;
			for (int t = slot+1; t < MAX_QUEUE; ++t) 
				pq[team].qm[t-1] = pq[team].qm[t];
		}
	}
	pq[team].split_queue();
}

void HockeyData::delLastPenalty(unsigned int team) {
	for (unsigned int t = MAX_QUEUE - 1; t >= 0; --t) {
		if (pq[team].qm[t] > 0) {
			delPenalty(team, t);
			break;
		}
	}
}

void HockeyData::editQueue(unsigned int team, std::string qstr) {

	// parse
	vector<string> vs;
	int nums[MAX_QUEUE] = { 0 };
	IniParser::trim(qstr);
	IniParser::remove_dup_delim(qstr, " ");
	IniParser::parsedelim(qstr, " ", vs);
	for (unsigned int i = 0; i < min(MAX_QUEUE, vs.size()); ++i) {
		nums[i] = IniParser::parseint(vs[i], 0);
	}

	// set
	int curr = invertTime(period, clock.read(), PERLEN, otlen);
	for ( int i = 0; i < MAX_QUEUE; ++i ) {
		if (nums[i] > 0 && nums[i] < 99) {
			if (i < 2) {
				int new_time = curr + nums[i] * 60000;
				if (pq[team].qm[i] <= 0 || pq[team].time[i] > new_time) {
					pq[team].time[i] = new_time;
				}	
			}
			pq[team].qm[i] = nums[i];
		}
		else {
			for (int j = i; j < MAX_QUEUE; ++j) pq[team].qm[j] = 0;	
			break;
		}
	}
	pq[team].split_queue();
}

void HockeyData::delPenaltyAuto() {
	unsigned char ac = 0; 
	unsigned char ai;
	for (int q = 0; q < 2; ++q) {
		for (int t = 0; t < 2; ++t) {
			if ( pq[q].active(t) ) { 
				++ac;
				ai = (q << 1) + t;
			}
		}
	}
	if ( ac == 1 ) delPenalty(ai >> 1, ai & 1);
}

void HockeyData::setPenaltyTime(unsigned int team, unsigned int slot, int per, int time, bool start) {
	if (team > 1 || slot > 1 || per < 1 || per > 9 || time < 0 || time > PERLEN) return;
	pq[team].time[slot] = invertTime(per, time, PERLEN, otlen) + (start ? (60000 * pq[team].qm[slot]) : 0);
	pq[team].split_queue();
}

void HockeyData::adjustPenaltyTime(unsigned int team, unsigned int slot, int time) {
	if (team > 1 || slot > 1) return;
	pq[team].time[slot] += time;
	pq[team].split_queue();
}

void HockeyData::updatePenalty() {
	int curr = invertTime(period, clock.read(), PERLEN, otlen);
	int low = 0x7fffffff;
	int rem;
	for (int q = 0; q < 2; ++q) {
		for (int t = 0; t < 2; ++t) {
			register short& qm = pq[q].qm[t];
			register unsigned short& qt = pq[q].qt[t];
			rem = 0;

			if (qm != 0) rem = pq[q].time[t] - curr;			// update clock

			// kill combinations (i.e. 4 = 2+2, 7 = 5+2, etc.)
			if (qm >= 4 && qm < 10 && !(qm & 1)) {
				if (rem <= (qm - 2) * 60000) qm -= 2;
			}
			else if (qm >= 7 && (qm & 1)) {
				if (rem <= (qm - 5) * 60000) qm -= 5;
			}

			// kill front of queue (and pop the next into the front) at right time
			if (qt != 0 && rem <= 0) {
				pq[q].pop_queue(t, curr, 0);
				//pq[q].time[t] += pq[q].qm[t] * 60000;
				//pq[q].split_queue();
				rem = pq[q].time[t] - curr;
			}

			// kill negative time after 5 seconds
			else if (qt == 0 && rem <= -5000 ) {
				qm = 0;
			}

			// negative minutes (only at end of timer, when only one should be in queue)
			else if ( rem <= 0 && qm > 0 ) qm = -(qm);
			else if ( rem > 0 && qm < 0 ) qm = -(qm);

			// don't show negative time
			if (qm <= 0 && qt == 0) {
				pq[q].rem_m[t] = 0;
				pq[q].rem_s[t] = 0;
			}
			else { 
				rem += qt * 60000;						// add queued time
				if ( rem % 1000 == 0 ) rem /= 1000;
				else rem = (rem + 1000) / 1000;
				if (rem < low) { 
					low = rem;
					pt_low_index = q*2 + t;
				}
				pq[q].rem_m[t] = rem / 60;
				pq[q].rem_s[t] = rem % 60;
			}
		}
	}
}

void HockeyData::getPenaltyInfo(unsigned short& vis, unsigned short& home, unsigned short& rmin, unsigned short& rsec) {
	updatePenalty();

	vis = 5;
	home = 5;
	
	if ( pq[0].qm[0] > 0 ) --vis;
	if ( pq[0].qm[1] > 0 ) --vis;
	if ( pq[1].qm[0] > 0 ) --home;
	if ( pq[1].qm[1] > 0 ) --home;

	rmin = rsec = 0;
	int q = pt_low_index >> 1;
	int t = pt_low_index & 1;
	rmin = pq[q].rem_m[t];
	rsec = pq[q].rem_s[t];

}

void HockeyData::printPenalties(std::string* disp, freetype::font_data* base) {
	stringstream ss;
	for (unsigned int q = 0; q <= 1; ++q) {
		for (unsigned int t = 0; t <= 1; ++t) {
			base->print(360.0f + 360.0f*q, 400.0f-20.0f*t, 0, "%d --%2hu:%02hu  %2d  +%u",
				t+1, pq[q].rem_m[t], pq[q].rem_s[t], pq[q].qm[t], pq[q].qt[t]);
		}
		for (int t = 2; t < MAX_QUEUE; ++t) {
			if (pq[q].qm[t] > 0) ss << pq[q].qm[t] << ' ';
			else break;
		}
		base->print(360.0f + 360.0f*q, 360.0f, 0, "Q -- %s", ss.str().c_str());
		ss.str("");

		//base->print(360.0f + 360.0f*q, 360.0f, 0, 
		//	"Q -- %hd %hd %hd %hd %hd %hd %hd %hd %hd %hd %hd %hd %hd %hd", 
		//	pq[q].qm[2], pq[q].qm[3], pq[q].qm[4], pq[q].qm[5], pq[q].qm[6], pq[q].qm[7], 
		//	pq[q].qm[8], pq[q].qm[9], pq[q].qm[10], pq[q].qm[11], pq[q].qm[12], pq[q].qm[13], 
		//	pq[q].qm[14], pq[q].qm[15]); 
	}
}