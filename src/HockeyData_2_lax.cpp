#ifdef LAX
#include "HockeyData.h"

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
			return "4";
			break;
		case 5:
			return "OT";
			break;
		case 127:
			return "FINAL";
			break;
		default:
			return std::string("OT").append(int2str(period - 4));
	}
}

void PenaltyQueue::split_queue() {
	int inc_time[3];
	inc_time[0] = time[0];
	inc_time[1] = time[1];
	inc_time[2] = time[2];
	qt[0] = qt[1] = qt[2] = 0;
	for (unsigned int q = 3; q < MAX_QUEUE; ++q) {
		if (!qm[q]) return;  // end of queue
		if (inc_time[0] <= inc_time[1] && inc_time[0] <= inc_time[2]) {
			qt[0] += qm[q];
			inc_time[0] += qm[q] * Q_LEN;
		}
		else if (inc_time[1] <= inc_time[0] && inc_time[1] <= inc_time[2]) {
			qt[1] += qm[q];
			inc_time[1] += qm[q] * Q_LEN;
		}
		else {
			qt[2] += qm[q];
			inc_time[2] += qm[q] * Q_LEN;
		}
	}
}

void HockeyData::delPenaltyAuto() {
	unsigned char ac[2] = {0}; 
	for (int q = 0; q < 2; ++q) {
		for (int t = 0; t < ACTIVE_QUEUE; ++t) {
			if ( pq[q].active(t) ) { 
				++ac[q];
			}
		}
	}
	if ( ac[0] > 0 && ac[1] == 0 ) {
		delPenalty(0, 0);
		delPenalty(0, 1);
		delPenalty(0, 2);
	}
	else if ( ac[1] > 0 && ac[0] == 0 ) {
		delPenalty(1, 0);
		delPenalty(1, 1);
		delPenalty(1, 2);
	}
}

void HockeyData::printPenalties(std::string* disp, freetype::font_data* base) {
	stringstream ss;
	for (unsigned int q = 0; q <= 1; ++q) {
		for (unsigned int t = 0; t < ACTIVE_QUEUE; ++t) {
			unsigned char c = ' ';
			if (pq[q].qm[t] & 1) c = 0xbd;
			unsigned char d = ' ';
			if (pq[q].qt[t] & 1) d = 0xbd;
			base->qprint(360.0f + 360.0f*q, 410.0f-20.0f*t, 0, "%d --%2hu:%02hu  %2d%c +%u%c",
				t+1, pq[q].rem_m[t], pq[q].rem_s[t], pq[q].qm[t]>>1, c, pq[q].qt[t]>>1, d);
		}
		// note: stops at 13, since that's all that fits.  s/b MAX_QUEUE with more room.
		for (int t = ACTIVE_QUEUE; t < 13; ++t) {
			if (pq[q].qm[t] > 0) ss << ' ' << (pq[q].qm[t] >> 1) << ((pq[q].qm[t] & 1) ? '\xbd' : ' ');
			else break;
		}
		// '+' hack
		if (pq[q].qm[13] > 0) ss << '+';
		base->qprint(360.0f + 360.0f*q, 350.0f, 0, "Q --%s", ss.str().c_str());
		ss.str("");
	}
}

#endif
