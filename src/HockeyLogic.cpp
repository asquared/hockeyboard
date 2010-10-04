#ifndef LAX
#include "HockeyLogic.h"
#include <iostream>

const char* HockeyLogic::penalties[] = { 
	"HITTING FROM BEHIND",
	"BOARDING",
	"CHARGING",
	"DELAY OF GAME", 
	"ELBOWING",
	"FIGHTING", 
	"GOALTENDER INT.", 
	"HOLDING",
	"INTERFERENCE", 
	"HOLDING THE STICK", 
	"HOOKING", 
	"CLIPPING",
	"TOO MANY MEN", 
	"KNEEING", 
	"OBSTRUCTION", 
	"SPEARING", 
	"BODY CHECKING", 
	"ROUGHING", 
	"SLASHING", 
	"TRIPPING", 
	"UNSPORTSMANLIKE", 
	"DIVING", 
	"HITTING AFTER WHISTLE", 
	"CROSS-CHECKING", 
	"HIGH STICKING" 
};

const char* HockeyLogic::gann[] = {
	"G: ",
	"A: ",
	"A: ",
	"",
	" PP",
	" SH"
};

const char* STR_TEAM_SEL = "Enter 1 or V for visiting team, 2 or H for home team:";

HockeyLogic::HockeyLogic() {
	inb[0] = inb[1] = inb[2] = inb[3] = 0;

	next = -1;
	for (int i = 0; i < 58; ++i) {
		funcarray[i] = NULL;
	}
	funcarray[1] = HockeyLogic::logicGAnn;					// Ctrl-A
	funcarray[3] = HockeyLogic::logicClock;					// Ctrl-C
	funcarray[7] = HockeyLogic::logicGoals;					// Ctrl-G
	funcarray[9] = HockeyLogic::logicIntClk;				// Ctrl-I
	funcarray[11] = HockeyLogic::logicKey;					// Ctrl-K
	funcarray[12] = HockeyLogic::logicStat;					// Ctrl-L
	funcarray[14] = HockeyLogic::logicNames;				// Ctrl-N
	funcarray[15] = HockeyLogic::logicColors;				// Ctrl-O
	funcarray[16] = HockeyLogic::logicPAnn;					// Ctrl-P
	funcarray[17] = HockeyLogic::logicPeriod;				// Ctrl-Q
	funcarray[18] = HockeyLogic::logicRoster;				// Ctrl-R
	funcarray[19] = HockeyLogic::logicStat;					// Ctrl-S
	funcarray[20] = HockeyLogic::logicPTime;				// Ctrl-T
	funcarray[25] = HockeyLogic::logicSync;					// Ctrl-Y
	funcarray[26] = HockeyLogic::logicReset;				// Ctrl-Z
	funcarray[27] = HockeyLogic::logicClear;				// ESC
	funcarray[30] = HockeyLogic::logicText;					// TAB
	funcarray[31] = HockeyLogic::logicQuit;					// Shift-ESC

}

double str2float(const string& s, double def) {
	stringstream ss;
	ss << s;
	double out = def;
	ss >> out;
	return out;
}

int str2int(const string& s, int def) {
	stringstream ss;
	ss << s;
	int out = def;
	ss >> out;
	return out;
}

int str2time(const string& s) {
	double in = str2float(s, -1.0f);
	if ( in < 0 ) return -1;
	double min = floor(in / 100.0);
	double sec = in - min * 100.0;
	return (int)(min * 60000.0 + sec * 1000.0 + 0.5);
}

int str2time_signed(const string& s) {
	double in = str2float(s, D_NAN);
	if (isnan(in)) return 0x80000000;
	int sign = in >= 0 ? 1 : -1;
	in = abs(in);
	double min = floor(in / 100.0);
	double sec = in - min * 100.0;
	return sign*(int)(min * 60000.0 + sec * 1000.0 + 0.5);
}

void HockeyLogic::draw_if(freetype::font_data* base) {
	// UI strings (0-41)
	float x, y;
	for ( int i = 0; i < 42; ++i ) {
		x = (float) ((i / 14) * 340 + 10);
		y = (float) (320 - (i % 14) * 20);
		base->qprint(x, y, 0, disp[i].c_str());
	}
	base->print(0, 40, 0, disp[42].c_str());
	if (isClear() || (timeGetTime() >> 9 & 1)) base->print(0, 20, 0, "%s", disp[43].c_str());
	else base->print(0, 20, 0, "%s%c", disp[43].c_str(), 0x15);
}

void HockeyLogic::clearStrings(int low, int high) {
	for (int i = low; i <= high; ++i) {
		disp[i] = "";
	}
}

void HockeyLogic::clear() {
	major = 0;
	minor = 0;
	disp[0] = "Ctrl-P: Penalty announce";
	disp[1] = "Ctrl-T: Penalty timers";
	disp[2] = "";
	disp[3] = "Ctrl-N: Set team names";
	disp[4] = "Ctrl-O: Set team colors";
	disp[5] = "Ctrl-R: Set team rosters";
	disp[6] = "Ctrl-S: Make statistics line";
	disp[7] = "";
	disp[8] = "Ctrl-G: Adjust goals/shots";
	disp[9] = "Ctrl-A: Announce goal";
	disp[10] = "";
	disp[11] = "Ctrl-K: Change key colors";
	disp[12] = "";
	disp[13] = "Shift-Esc: Exit program";
	disp[14] = "Ctrl-C: Adjust clock";
	disp[15] = "Ctrl-I: Toggle intermission clock";
	disp[16] = "Ctrl-Q: Adjust period";
	disp[17] = "Ctrl-Y: Synchronization settings";
	clearStrings(18,24);
	disp[25] = "Ctrl-Z: Reset all";
	clearStrings(26,43);
	// clear any locals necessary
	inb[0] = inb[1] = inb[2] = inb[3] = 0;
	ini = 0;
	input = "";
}

void HockeyLogic::logicClear(HockeyData* data, HockeyDraw* hd, Keybuffer* kbuf, HockeyDrop* drop) {
	clear();
}

void HockeyLogic::selectMajor(HockeyData* data, HockeyDraw* hd, Keybuffer* kbuf, HockeyDrop* drop) {
	if (next != -1 && next > 0 && next < 58) {
		if ( funcarray[next] != NULL ) major = next;
	}

	next = -1;
	if (major > 0 && major < 58) {
		if ( funcarray[major] != NULL ) (this->*funcarray[major])(data, hd, kbuf, drop);
	}

}

void HockeyLogic::logicClock(HockeyData* data, HockeyDraw* hd, Keybuffer* kbuf, HockeyDrop* drop) {
	char key;
	switch (minor) {
		case 0:
			disp[0] = "S: set clock";
			disp[1] = "A: adjust clock";
			disp[2] = "R: reset clock";
			disp[3] = "O: set overtime length";
			clearStrings(4,16);
			char buf[8];
			snprintf(buf, 8, "%d:%02d", data->otlen / 60000, (data->otlen % 60000) / 1000);
			disp[17] = buf;
			clearStrings(18,41);
			disp[42] = "Enter choice:";
			disp[43] = "";
			minor = 1;
			break;
		case 1:
			key = kbuf->last();
			if ( key >= 'A' && key <= 'Z' ) key += 32;
			if ( key == 'a' ) {
				clearStrings(0,41);
				disp[42] = "Enter time to adjust by, in [-]MMSS.D format:";
				minor = 10;
			}
			else if (key == 'o') {
				clearStrings(0,41);
				disp[42] = "Enter length of overtime in minutes:";
				minor = 20;
			}
			else if (key == 's') {
				clearStrings(0,41);
				disp[42] = "Enter time to set to, in MMSS.D format:";
				minor = 30;
			}
			else if (key == 'r') {
				clearStrings(0,41);
				disp[42] = "Enter time to reset to in minutes:";
				minor = 40;
			}
			kbuf->clear();
			break;
		case 10:
			disp[43] = kbuf->fullbuf();
			if ( kbuf->enter() ) {
				ini = str2time_signed(kbuf->fullbuf());
				if (ini != 0x80000000) {
					data->active_clock->adjust(ini);
					clear();
				}
				disp[43] = "";
				kbuf->clear();
			}
			break;
		case 20:
			disp[43] = kbuf->fullbuf();
			if ( kbuf->enter() ) {
				double in = str2float( kbuf->fullbuf(), -1.0 );
				if ( in > 0 ) {
					data->otlen = (int) (in * 60000.0);
					clear();
				}
				disp[43] = "";
				kbuf->clear();
			}
			break;
		case 30:
			disp[43] = kbuf->fullbuf();
			if ( kbuf->enter() ) {
				ini = str2time(kbuf->fullbuf());
				if (ini >= 0) {
					data->active_clock->set(ini);
					clear();
				}
				disp[43] = "";
				kbuf->clear();
			}
			break;
		case 40:
			disp[43] = kbuf->fullbuf();
			if ( kbuf->enter() ) {
				double in = str2float(kbuf->fullbuf(), -1.0);
				if ( in >= 0 ) {
					int time = (int)(in * 60000.0);
					data->active_clock->set(time);
					clear();
				}
				disp[43] = "";
				kbuf->clear();
			}
			break;
	}
}	

void HockeyLogic::logicIntClk(HockeyData* data, HockeyDraw* hd, Keybuffer* kbuf, HockeyDrop* drop) {
	if ( data->active_clock == &(data->clock) ) data->active_clock = &(data->int_clock);
	else data->active_clock = &(data->clock);
	clear();
}

void HockeyLogic::logicGoals(HockeyData* data, HockeyDraw* hd, Keybuffer* kbuf, HockeyDrop* drop) {
	int key;
	switch (minor) {
		case 0:
			clearStrings(0,43);
			disp[42] = STR_TEAM_SEL;
			minor = 1;
			kbuf->clear();
			break;
		case 1:
			key = kbuf->last();
			if ( key >= 'A' && key <= 'Z') key += 32;
			if ( key == 'v' ) key = '1';
			else if ( key == 'h') key = '2';
			if ( key == '1' || key == '2' ) {
				inb[0] = key - '1';
				disp[10] = "G: goals";
				disp[11] = "S: shots on goal";
				disp[42] = "Enter choice:";
				minor = 2;
			}
			kbuf->clear();
			break;
		case 2:
			key = kbuf->last();
			if ( key >= 'A' && key <= 'Z') key += 32;
			if ( key == 'g' ) {
				clearStrings(10,11);
				disp[42] = "Enter number of goals:";
				minor = 10;
			}
			else if ( key == 's' ) {
				clearStrings(10,11);
				disp[42] = "Enter number of shots on goal:";
				minor = 20;
			}
			kbuf->clear();
			break;
		case 10:
			clearStrings(0,41);
			disp[43] = kbuf->fullbuf();
			if ( kbuf->enter() ) {
				inb[1] = str2int(disp[43], -1);
				if ( inb[1] != 0xff ) {
					data->tm[inb[0]].sc = inb[1];
					clear();
				}
				disp[43] = "";
				kbuf->clear();
			}
			break;
		case 20:
			clearStrings(0,41);
			disp[43] = kbuf->fullbuf();
			if ( kbuf->enter() ) {
				inb[1] = str2int(disp[43], -1);
				if ( inb[1] != 0xff ) {
					data->tm[inb[0]].sog = inb[1];
					clear();
				}
				disp[43] = "";
				kbuf->clear();
			}
			break;
	}
}

void HockeyLogic::logicGAnn(HockeyData* data, HockeyDraw* hd, Keybuffer* kbuf, HockeyDrop* drop) {
	int key;
	switch (minor) {
		case 0:
			clearStrings(0,43);
			disp[8] = "P: power play goal";
			disp[9] = "S: shorthanded goal";
			disp[10] = "E: even strength goal";
			disp[42] = STR_TEAM_SEL;
			inb[1] = 0;
			minor = 1;
			kbuf->clear();
			break;
		case 1:
			key = kbuf->last();
			if ( key >= 'A' && key <= 'Z' ) key += 32;
			if ( key == 'p' ) {
				inb[1] = 1;
				disp[12] = "POWER PLAY";
			}
			else if ( key == 's' ) {
				inb[1] = 2;
				disp[12] = "SHORTHANDED";
			}
			else if ( key == 'e' ) {
				inb[1] = 0;
				disp[12] = "";
			}
			else if ( key == 'v' ) key = '1';
			else if ( key == 'h') key = '2';
			if ( key == '1' || key == '2' ) {
				inb[0] = key - '1';
				clearStrings(0,43);
				disp[42] = "Enter player that scored goal, and any credited with assists (Format: G [A] [A]):";
				minor = 2;
			}
			kbuf->clear();
			break;
		case 2:
			clearStrings(0,41);
			disp[43] = kbuf->fullbuf();
			if ( kbuf->enter() ) {
				// move up
				for (unsigned int i = 42; i < 46; ++i) drop->setstring(i, drop->getstring(i+4));
				// parse
				vector<string> vs;
				int nums[3] = { -1, -1, -1 };
				string buf = kbuf->fullbuf();
				IniParser::trim(buf);
				IniParser::remove_dup_delim(buf, " ");
				IniParser::parsedelim(buf, " ", vs);
				for (unsigned int i = 0; i < min(3, vs.size()); ++i) {
					nums[i] = str2int(vs[i], -1);
					if ( nums[i] != -1 ) 
						drop->setstring(47+i, string(gann[i]) + "#" + vs[i] + " " + 
						data->rl->get(data->tm[inb[0]].rs, nums[i]) );
					else drop->setstring(47+i, string(""));
				}
				drop->setstring(46, data->tm[inb[0]].lname + gann[3+inb[1]] + " GOAL");
				kbuf->clear();
				clear();
			}
			break;
	}
}

void HockeyLogic::logicKey(HockeyData* data, HockeyDraw* hd, Keybuffer* kbuf, HockeyDrop* drop) {
	int key;
	switch (minor) { 
		case 0: 
			clearStrings(0,43);
			disp[0] = "B: blue chroma key";
			disp[1] = "G: green chroma key";
			disp[2] = "L: luma key";
			disp[4] = "X: external key (white bar)";
			disp[14] = "+: move bar up";
			disp[15] = "-: move bar down";
			disp[16] = "0: reset bar to default position";
			disp[18] = "Y offset: " + IniParser::int2str(hd->offset);
			disp[42] = "Press letter to select keying mode";
			minor = 1;
			kbuf->clear();
			break;
		case 1:
			key = kbuf->last();
			if ( key == '=' || key == '+' ) {
				hd->offset += 1;
				if (hd->offset > 32) hd->offset = 32;
				disp[18] = "Y offset: " + IniParser::int2str(hd->offset);
			}
			else if (key == '-' || key == '_') {
				hd->offset -= 1;
				if (hd->offset < -64) hd->offset = -64;
				disp[18] = "Y offset: " + IniParser::int2str(hd->offset);
			}
			else if (key == '0') {
				hd->offset = 0;
				disp[18] = "Y offset: " + IniParser::int2str(hd->offset);
			}
			if ( key >= 'A' && key <= 'Z' ) key += 32;
			if ( key == 'b' || key == 'g' || key == 'l' || key == 'x' ) {
				data->keymode = key;
				SetCursor(NULL);		// kind of a temporary fix
				clear();
			}
			kbuf->clear();
			break;
	}
}

void HockeyLogic::logicNames(HockeyData* data, HockeyDraw* hd, Keybuffer* kbuf, HockeyDrop* drop) {
	int key;
	switch (minor) {
		case 0:
			clearStrings(0,43);
			disp[42] = STR_TEAM_SEL;
			disp[7] = "1N: " + data->tm[0].name;
			disp[8] = "1L: " + data->tm[0].lname;
			disp[9] = "1F: " + data->tm[0].fname;
			disp[21] = "2N: " + data->tm[1].name;
			disp[22] = "2L: " + data->tm[1].lname;
			disp[23] = "2F: " + data->tm[1].fname;
			minor = 1;
			kbuf->clear();
			break;
		case 1:
			key = kbuf->last();
			if ( key >= 'A' && key <= 'Z') key += 32;
			if ( key == 'v' ) key = '1';
			else if ( key == 'h') key = '2';
			if ( key == '1' || key == '2' ) {
				inb[0] = key - '1';
				disp[42] = "Select name type:";
				disp[11] = "N: short name";
				disp[12] = "L: long name";
				disp[13] = "F: full name";
				minor = 2;
			}
			kbuf->clear();
			break;
		case 2:
			key = kbuf->last();
			if ( key >= 'A' && key <= 'Z') key += 32;
			if (key == 'n') {
				clearStrings(11,13);
				disp[42] = "Enter team short name (3-4 char.)";
				minor = 10;
			}
			else if (key == 'l') {
				clearStrings(11,13);
				disp[42] = "Enter team long name (3-6 char.):";
				minor = 20;
			}
			else if (key == 'f') {
				clearStrings(11,13);
				disp[42] = "Enter team full name:";
				minor = 30;
			}
			kbuf->clear();
			break;
		case 10:
			disp[43] = kbuf->fullbuf();
			if ( kbuf->enter() ) {
				data->tm[inb[0]].name = kbuf->fullbuf();
				if (data->tm[inb[0]].lname == "") {
					string tostr = kbuf->fullbuf() + " TIMEOUT";
					drop->setstring(SI_TO_V+inb[0], tostr);
				}
				kbuf->clear();
				clear();
			}
			break;
		case 20:
			disp[43] = kbuf->fullbuf();
			if ( kbuf->enter() ) {
				data->tm[inb[0]].lname = kbuf->fullbuf();
				string tostr = kbuf->fullbuf() + " TIMEOUT";
				drop->setstring(SI_TO_V+inb[0], tostr);
				kbuf->clear();
				clear();
			}
			break;
		case 30:
			disp[43] = kbuf->fullbuf();
			if ( kbuf->enter() ) {
				data->tm[inb[0]].fname = kbuf->fullbuf();
				kbuf->clear();
				clear();
			}
			break;
	}
}

void HockeyLogic::logicColors(HockeyData* data, HockeyDraw* hd, Keybuffer* kbuf, HockeyDrop* drop) {
	int key;
	bool valid = false;
	switch (minor) {
		case 0:
			clearStrings(0,43);
			disp[42] = "Enter 1 or V for visiting team, 2 or H for home team:";
			minor = 1;
			kbuf->clear();
			break;
		case 1:
			key = kbuf->last();
			if ( key >= 'A' && key <= 'Z') key += 32;
			if ( key == 'v' ) key = '1';
			else if ( key == 'h') key = '2';
			if ( key == '1' || key == '2' ) {
				inb[0] = key - '1';
				disp[42] = "Enter background color (Format: R G B):";
				minor = 20;
			}
			kbuf->clear();
			break;
		case 20:
			disp[43] = kbuf->fullbuf();
			if ( kbuf->fullbuf().size() > 96 ) kbuf->add(8);
			if ( kbuf->enter() ) {
				vector<string> vs;
				string buf = kbuf->fullbuf();
				IniParser::trim(buf);
				IniParser::remove_dup_delim(buf, " ");
				IniParser::parsedelim(buf, " ", vs);
				if (vs.size() == 3) {
					int r = str2int(vs[0], -1);
					int g = str2int(vs[1], -1);
					int b = str2int(vs[2], -1);
					if (r >= 0 && r <= 255 && g >= 0 && g <= 255 && b >= 0 && b <= 255) {
						hd->update_team_color(inb[0], r, g, b);
					}
				}
				disp[42] = "Enter text color (Format: R G B):";
				disp[43] = "";
				kbuf->clear();
				minor = 21;
			}
			break;
		case 21:
			disp[43] = kbuf->fullbuf();
			if ( kbuf->fullbuf().size() > 96 ) kbuf->add(8);
			if ( kbuf->enter() ) {
				vector<string> vs;
				string buf = kbuf->fullbuf();
				IniParser::trim(buf);
				IniParser::remove_dup_delim(buf, " ");
				IniParser::parsedelim(buf, " ", vs);
				if (vs.size() == 3) {
					int r = str2int(vs[0], -1);
					int g = str2int(vs[1], -1);
					int b = str2int(vs[2], -1);
					if (r >= 0 && r <= 255 && g >= 0 && g <= 255 && b >= 0 && b <= 255) {
						hd->name[inb[0]]->color(r,g,b);
					}
				}
				kbuf->clear();
				clear();
			}
			break;
	}
}

void HockeyLogic::logicRoster(HockeyData* data, HockeyDraw* hd, Keybuffer* kbuf, HockeyDrop* drop) {
	int key;
	switch (minor) {
		case 0:
			clearStrings(0,43);
			disp[12] = "R: reload roster file";
			disp[23] = "Number of rosters loaded: " + IniParser::int2str(data->rl->team_count());
			disp[25] = "V: " + data->tm[0].rs + " (" + IniParser::int2str(data->rl->count(data->tm[0].rs)) + ")";
			disp[26] = "H: " + data->tm[1].rs + " (" + IniParser::int2str(data->rl->count(data->tm[1].rs)) + ")";
			disp[42] = STR_TEAM_SEL;
			minor = 1;
			kbuf->clear();
			break;
		case 1:
			key = kbuf->last();
			if ( key >= 'A' && key <= 'Z') key += 32;
			if ( key == 'v' ) key = '1';
			else if ( key == 'h') key = '2';
			if ( key == '1' || key == '2' ) {
				inb[0] = key - '1';
				disp[12] = "";
				disp[42] = "Enter roster name from roster.ini:";
				minor = 2;
			}
			else if ( key == 'r') {
				data->reloadRosters();
				clear();
			}
			kbuf->clear();
			break;
		case 2:
			disp[43] = kbuf->fullbuf();
			if ( kbuf->enter() ) {
				data->tm[inb[0]].rs = kbuf->fullbuf();
				kbuf->clear();
				clear();
			}
			break;
	}
}

void HockeyLogic::logicPAnn(HockeyData* data, HockeyDraw* hd, Keybuffer* kbuf, HockeyDrop* drop) {
	int key;
	switch (minor) {
		case 0:
			clearStrings(0,43);
			ini = data->clock_last_stopped;
			disp[42] = STR_TEAM_SEL;
			minor = 1;
			kbuf->clear();
			break;
		case 1:
			disp[43] = "";
			key = kbuf->last();
			if ( key >= 'A' && key <= 'Z') key += 32;
			if ( key == 'v' ) key = '1';
			else if ( key == 'h') key = '2';
			if ( key == '1' || key == '2' ) {
				inb[0] = key - '1';
				disp[12] = "B: bench minor";
				disp[42] = "Enter optional player number:";
				minor = 2;
			}
			kbuf->clear();
			break;
		case 2:
			disp[43] = kbuf->fullbuf();
			if ( kbuf->enter() ) {
				clearStrings(0,43);
				inb[1] = str2int(kbuf->fullbuf(), 255);
				if (kbuf->fullbuf()[0] == 'b' || kbuf->fullbuf()[0] == 'B') {
					inb[1] = 254;
				}
				disp[14] = "B: boarding";
				disp[15] = "Q: body checking (women only)";
				disp[16] = "C: charging";
				disp[17] = "L: clipping";
				disp[18] = "X: cross-checking";
				disp[19] = "D: delay of game";
				disp[20] = "V: diving";
				disp[21] = "E: elbowing";
				disp[22] = "F: fighting";
				disp[23] = "G: goaltender interference";
				disp[24] = "Y: high sticking";
				disp[25] = "W: hitting after whistle";
				disp[26] = "A: hitting from behind";
				disp[27] = "H: holding";
				disp[28] = "J: holding the stick";
				disp[29] = "K: hooking";
				disp[30] = "I: interference";
				disp[31] = "N: kneeing";
				disp[32] = "O: obstruction";
				disp[33] = "R: roughing";
				disp[34] = "S: slashing";
				disp[35] = "P: spearing";
				disp[36] = "M: too many men";
				disp[37] = "T: tripping";
				disp[38] = "U: unsportsmanlike conduct";
				disp[41] = "Z: (custom)";
				disp[42] = "Select penalty letter (Z for custom):";
				kbuf->clear();
				minor = 3;
			}
			break;
		case 3:
			disp[43] = "";
			key = kbuf->last();
			if ( key >= 'A' && key <= 'Z' ) key += 32;
			if ( key >= 'a' && key <= 'z') {
				if ( key == 'z' ) {
					clearStrings(0,41);
					disp[42] = "Type name of penalty:";
					minor = 4;
				}
				else {
					input = penalties[key - 'a'];
					clearStrings(0,41);
					disp[0] = "2: minor (2 min) penalty";
					disp[1] = "4: double minor (4 min) penalty";
					disp[2] = "5: major (5 min) penalty";
					disp[4] = "Enter: do not load into timers";
					disp[42] = "Select number of minutes to load penalty into penalty timers:";
					minor = 5;
				}
			}
			kbuf->clear();
			break;
		case 4:
			disp[43] = kbuf->fullbuf();
			if ( kbuf->enter() ) {
				input = kbuf->fullbuf();
				clearStrings(3,43);
				disp[0] = "2: minor (2 min) penalty";
				disp[1] = "4: double minor (4 min) penalty";
				disp[2] = "5: major (5 min) penalty";
				disp[4] = "Enter: do not load into timers";
				disp[42] = "Select number of minutes to load penalty into penalty timers:";
				kbuf->clear();
				minor = 5;
			}
			break;
		case 5:
			disp[43] = "";
			key = kbuf->last();
			unsigned char min = 0;
			if ( key >= '1' && key <= '9') min = key - '0';
			if ( min != 0 || kbuf->enter() ) {
				if ( min != 0 ) data->addPenalty( inb[0], data->period, ini, min );
				for (int i = 33; i <= 41; ++i) drop->setstring(i-3, drop->getstring(i));
				drop->setstring(39, data->tm[inb[0]].lname + " PENALTY");
				drop->setstring(40, input);
				drop->setstring(41, input);
				string player_name = data->rl->get(data->tm[inb[0]].rs, inb[1]);
				if (player_name != "") {
					player_name = "#" + IniParser::int2str(inb[1]) + " " + player_name;
					drop->setstring(40, player_name);
				}
				else if (inb[1] == 254) {
					player_name = "BENCH MINOR";
					drop->setstring(40, player_name);
				}
				clear();
			}
			kbuf->clear();
			break;
	}

}

void HockeyLogic::logicPTime(HockeyData* data, HockeyDraw* hd, Keybuffer* kbuf, HockeyDrop* drop) {
	int key;
	switch (minor) {
		case 0:
			disp[42] = "Enter letter:";
			disp[43] = "";
			disp[0] = "C: create";
			disp[1] = "D: delete single penalty from queue";
			disp[2] = "L: delete last penalty in queue";
			disp[3] = "T: delete entire team's penalties";
			disp[4] = "Q: edit queue";
			disp[5] = "";
			disp[6] = "S: set start time of penalty";
			disp[7] = "E: set end time of penalty";
			disp[8] = "A: adjust time of penalty";
			disp[9] = "";
			disp[10] = "G: goal scored, auto-delete if possible";
			clearStrings(11,41);
			minor = 1;
			kbuf->clear();
			break;
		case 1:
			key = kbuf->last();
			if ( key >= 'A' && key <= 'Z' ) key += 32;
			if ( key == 'c' ) {
				clearStrings(0,43);
				disp[42] = STR_TEAM_SEL; 
				minor = 10;
			}
			else if ( key == 't' ) { 
				clearStrings(0,43);
				disp[42] = STR_TEAM_SEL;
				minor = 20;
			}
			else if ( key == 'd' ) {
				clearStrings(0,43);
				disp[42] = STR_TEAM_SEL;
				minor = 30;
			}
			else if ( key == 'l' ) {
				clearStrings(0,43);
				disp[42] = STR_TEAM_SEL;
				minor = 40;
			}
			else if ( key == 'q' ) {
				clearStrings(0,43);
				disp[42] = STR_TEAM_SEL;
				minor = 50;
			}
			else if ( key == 's' || key == 'e' ) {
				if ( key == 's' ) inb[3] = 1;
				clearStrings(0,43);
				disp[42] = STR_TEAM_SEL;
				minor = 60;
			}
			else if ( key == 'a' ) {
				clearStrings(0,43);
				disp[42] = STR_TEAM_SEL;
				minor = 70;
			}
			else if ( key == 'g' ) {
				data->delPenaltyAuto();
				clear();
			}
			kbuf->clear();
			break;
		case 10:
			key = kbuf->last();
			if ( key >= 'A' && key <= 'Z') key += 32;
			if ( key == 'v' ) key = '1';
			else if ( key == 'h') key = '2';
			if ( key == '1' || key == '2' ) {
				inb[0] = key - '1';
				disp[42] = "Enter starting period number (1-9), or press N for current time:";
				minor = 11;
			}
			kbuf->clear();
			break;
		case 11:
			key = kbuf->last();
			if ( key == 'n' || key == 'N' ) {
				inb[1] = 0;
				disp[42] = "Enter penalty time in minutes:";
				disp[43] = "";
				minor = 13;
			}
			if ( key >= '1' && key <= '9' ) {
				inb[1] = key - '0';
				disp[42] = "Enter start time, in MMSS.D format:";
				minor = 12;
			}
			kbuf->clear();
			break;
		case 12:
			disp[43] = kbuf->fullbuf();
			if ( kbuf->enter() ) {
				int time = str2time(kbuf->fullbuf());
				if ( time >= 0 ) {
					ini = time;
					minor = 13;
					disp[42] = "Enter penalty time in minutes:";
				}
				disp[43] = "";
				kbuf->clear();
			}
			break;
		case 13:
			key = kbuf->last();
			if ( key >= '1' && key <= '9' ) {
					key -= '0';
				if ( inb[1] > 0 ) data->addPenalty( inb[0], inb[1], ini, key );
				else if ( inb[1] == 0 ) data->addPenalty( inb[0], data->period, data->clock.read(), key );
				clear();
			}
			kbuf->clear();
			break;
		case 20:
			key = kbuf->last();
			if ( key >= 'A' && key <= 'Z') key += 32;
			if ( key == 'v' ) key = '1';
			else if ( key == 'h') key = '2';
			if ( key == '1' || key == '2' ) {
				key -= '1';
				data->delPenalty(key);
				clear();
			}
			kbuf->clear();
			break;
		case 30:
			key = kbuf->last();
			if ( key >= 'A' && key <= 'Z') key += 32;
			if ( key == 'v' ) key = '1';
			else if ( key == 'h') key = '2';
			if ( key == '1' || key == '2' ) {
				inb[0] = key - '1';
				disp[42] = "Enter slot number (1-16):";
				minor = 31;
			}
			kbuf->clear();
			break;
		case 31:
			disp[43] = kbuf->fullbuf();
			if ( kbuf->enter() ) {
				inb[1] = str2int(kbuf->fullbuf(), -1);
				if (inb[1] >= 1 && inb[1] <= 16) {
					data->delPenalty(inb[0], inb[1]-1);
					clear();
				}
				disp[43] = "";
				kbuf->clear();
			}
			break;
		case 40:
			key = kbuf->last();
			if ( key >= 'A' && key <= 'Z') key += 32;
			if ( key == 'v' ) key = '1';
			else if ( key == 'h') key = '2';
			if ( key == '1' || key == '2' ) {
				key -= '1';
				data->delLastPenalty(key);
				clear();
			}
			kbuf->clear();
			break;
		case 50:
			key = kbuf->last();
			if ( key >= 'A' && key <= 'Z') key += 32;
			if ( key == 'v' ) key = '1';
			else if ( key == 'h') key = '2';
			if ( key == '1' || key == '2' ) {
				inb[0] = key - '1';
				disp[42] = "Enter new queue string (e.g. \"2 2 5 2 0 0 0 0\")";
				minor = 51;
			}
			kbuf->clear();
			break;
		case 51:
			disp[43] = kbuf->fullbuf();
			if ( kbuf->enter() ) {
				data->editQueue(inb[0], kbuf->fullbuf());
				kbuf->clear();
				clear();
			}
			break;
		case 60:
			key = kbuf->last();
			if ( key >= 'A' && key <= 'Z') key += 32;
			if ( key == 'v' ) key = '1';
			else if ( key == 'h') key = '2';
			if ( key == '1' || key == '2' ) {
				inb[0] = key - '1';
				disp[42] = "Enter slot number (1-2):";
				minor = 61;
			}
			kbuf->clear();
			break;
		case 61:
			disp[43] = kbuf->fullbuf();
			if ( kbuf->enter() ) {
				inb[1] = str2int(kbuf->fullbuf(), -1) - 1;
				if (inb[1] < 2) {
					disp[42] = "Enter period number (1-9), or press N for current time:";
					minor = 62;
				}
				disp[43] = "";
				kbuf->clear();
			}
			break;
		case 62:
			key = kbuf->last();
			if ( key == 'n' || key == 'N' ) {
				inb[1] = 0;
				data->setPenaltyTime(inb[0], inb[1], data->period, data->clock.read(), inb[3] != 0);
				clear();
			}
			if ( key >= '1' && key <= '9' ) {
				inb[2] = key - '0';
				disp[42] = "Enter ";
				disp[42] += (inb[3] ? "start" : "end");
				disp[42] += " time, in MMSS.D format:";
				minor = 63;
			}
			kbuf->clear();
			break;
		case 63:
			disp[43] = kbuf->fullbuf();
			if ( kbuf->enter() ) {
				ini = str2time(kbuf->fullbuf());
				if ( ini >= 0 ) {
					data->setPenaltyTime(inb[0], inb[1], inb[2], ini, inb[3] != 0);
					clear();
				}
				disp[43] = "";
				kbuf->clear();
			}
			break;
		case 70:
			key = kbuf->last();
			if ( key >= 'A' && key <= 'Z') key += 32;
			if ( key == 'v' ) key = '1';
			else if ( key == 'h') key = '2';
			if ( key == '1' || key == '2' ) {
				inb[0] = key - '1';
				disp[42] = "Enter slot number (1-2):";
				minor = 71;
			}
			kbuf->clear();
			break;
		case 71:
			disp[43] = kbuf->fullbuf();
			if ( kbuf->enter() ) {
				inb[1] = str2int(kbuf->fullbuf(), -1) - 1;
				if (inb[1] < 2) {
					disp[42] = "Enter time to adjust by, in [-]MMSS.D format:";
					minor = 72;
				}
				disp[43] = "";
				kbuf->clear();
			}
			break;
		case 72:
			disp[43] = kbuf->fullbuf();
			if ( kbuf->enter() ) {
				ini = str2time_signed(kbuf->fullbuf());
				if ( ini != 0x80000000 ) {
					data->adjustPenaltyTime(inb[0], inb[1], ini);
					clear();
				}
				disp[43] = "";
				kbuf->clear();
			}
			break;
	}
}

void HockeyLogic::logicPeriod(HockeyData* data, HockeyDraw* hd, Keybuffer* kbuf, HockeyDrop* drop) {
	int key;
	switch (minor) {
		case 0:
			disp[42] = "Enter number of period or letter:";
			disp[43] = "";
			disp[0] = "O: Overtime (period 4)";
			disp[1] = "F: Final";
			disp[2] = "";
			disp[3] = "A: advance to next period and reset clock";
			clearStrings(4,41);
			minor = 1;
			kbuf->clear();
			break;
		case 1:
			key = kbuf->last();
			if ( key >= 'A' && key <= 'Z' ) key += 32;
			if (key >= '1' && key <= '9') {
				data->period = key - '0';
				clear();
			}
			else if (key == 'a') {
				++(data->period);
				if ( data->period > 127 ) data->period = 1;
				else if ( data->period > 9 ) data->period = 9;
				if ( data->period < 4 ) data->clock.set(data->perlen);
				else data->clock.set(data->otlen);
				clear();
			}
			else if (key == 'f') {
				data->period = FINAL;
				clear();
			}
			else if (key == 'o') {
				data->period = 4;
				clear();
			}
			kbuf->clear();
			break;
	}
}

void HockeyLogic::logicReset(HockeyData* data, HockeyDraw* hd, Keybuffer* kbuf, HockeyDrop* drop) {
	int key;
	switch (minor) {
		case 0:
			clearStrings(0,43);
			disp[42] = "Are you sure you want to reset the score and clock? Press Y to reset.";
			minor = 1;
			kbuf->clear();
			break;
		case 1:
			key = kbuf->last();
			if (key == 'y' || key == 'Y') {
				data->clock.set(data->perlen);
				data->period = 1;
				for ( int i = 0; i < 2; ++i ) {
					data->delPenalty(i);
					data->tm[i].sc = 0;
					data->tm[i].sog = 0;
				}
				data->pt_low_index = 0;
			}
			kbuf->clear();
			clear();
			break;
	}
}

void HockeyLogic::logicText(HockeyData* data, HockeyDraw* hd, Keybuffer* kbuf, HockeyDrop* drop) {
	switch (minor) {
		case 0:
			clearStrings(0,43);
			disp[42] = "Enter number of text slot (0-57):";
			minor = 1;
			kbuf->clear();
			break;
		case 1:
			disp[43] = kbuf->fullbuf();
			if ( kbuf->enter() ) {
				inb[0] = str2int(kbuf->fullbuf(), -1);
				if (inb[0] >= MIN_USER_STATE && inb[0] <= MAX_WRITE_STATE) {
					disp[42] = "Enter text string for slot " + kbuf->fullbuf() + ":";
					minor = 2;
				}
				disp[43] = "";
				kbuf->clear();
			}
			break;
		case 2:
			clearStrings(0,41);
			disp[43] = kbuf->fullbuf();
			if ( kbuf->enter() ) {
				drop->setstring(inb[0], kbuf->fullbuf());
				kbuf->clear();
				clear();
			}
			break;
	}
}

void HockeyLogic::logicStat(HockeyData* data, HockeyDraw* hd, Keybuffer* kbuf, HockeyDrop* drop) {
	switch (minor) {
		case 0:
			clearStrings(0,43);
			disp[42] = "Enter number of text slot (0-57):";
			minor = 1;
			kbuf->clear();
			break;
		case 1:
			disp[43] = kbuf->fullbuf();
			if ( kbuf->enter() ) {
				inb[0] = str2int(kbuf->fullbuf(), -1);
				if (inb[0] >= MIN_USER_STATE && inb[0] <= MAX_WRITE_STATE) {
					disp[42] = "Enter statistics values for teams (Format: V H):";
					minor = 2;
				}
				disp[43] = "";
				kbuf->clear();
			}
			break;
		case 2:
			clearStrings(0,41);
			disp[43] = kbuf->fullbuf();
			if ( kbuf->enter() ) {
				vector<string> vs;  // parser output
				string buf = kbuf->fullbuf();
				IniParser::trim(buf);
				IniParser::remove_dup_delim(buf, " ");
				IniParser::parsedelim(buf, " ", vs);
				while (vs.size() < 2) vs.push_back(" ");
				drop->setstring(inb[0], data->tm[0].lname + ": " + vs[0] + "  " + 
					data->tm[1].lname + ": " + vs[1]);
				kbuf->clear();
				clear();
			}
			break;
	}
}

void HockeyLogic::logicSync(HockeyData* data, HockeyDraw* hd, Keybuffer* kbuf, HockeyDrop* drop) {
	int key, port;
	switch (minor) {
		case 0:
			disp[42] = "Enter choice:";
			disp[43] = "";
			disp[0] = "P: Select port";
			disp[1] = "S: Scan for port";
			disp[2] = "";
			disp[3] = "D: Set transition delays";
			disp[4] = "";
			disp[5] = "M: Switch sync mode";
			disp[6] = "";
			disp[7] = "T: Use tenths of seconds?";
			clearStrings(8,41);
			disp[17] = int2str(data->start_delay) + "/" + int2str(data->stop_delay);
			disp[19] = (data->sync_tr) ? "READ 7-SEG CLOCK" : "READ GREEN LIGHT";
			disp[21] = (data->use_tenths) ? "YES" : "NO";
			port = data->ssync.get_port();
			if (port == 0) disp[10] = "Currently using port: NONE";
			else disp[10] = "Currently using port: COM" + int2str(port);
			minor = 1;
			kbuf->clear();
			break;
		case 1:
			key = kbuf->last();
			if ( key >= 'A' && key <= 'Z' ) key += 32;
			if (key == 'p') {
				disp[42] = "Enter number of COM port:";
				clearStrings(0,7);
				clearStrings(17,21);
				minor = 10;
			}
			else if (key == 's') {
				clearStrings(0,41);
				disp[42] = "Scanning...";
				inb[0] = data->ssync.port_scan();
				if (inb[0] != 0) disp[10] = "Data found on COM" + int2str(inb[0]);
				else disp[10] = "No data found on any port";
				disp[42] = "Press any key to return to menu";
				minor = 20;
			}
			else if (key == 'd') {
				clearStrings(0,41);
				disp[42] = "Enter stop->start (on->off) delay in milliseconds:";
				minor = 30;
			}
			else if (key == 'm') {
				data->sync_tr = !data->sync_tr;
				disp[19] = (data->sync_tr) ? "READ 7-SEG CLOCK" : "READ GREEN LIGHT";
			}
			else if (key == 't') {
				data->use_tenths = !data->use_tenths;
				disp[21] = (data->use_tenths) ? "YES" : "NO";
			}
			kbuf->clear();
			break;
		case 10:
			disp[43] = kbuf->fullbuf();
			if ( kbuf->enter() ) {
				inb[0] = str2int(kbuf->fullbuf(), -1);
				if (inb[0] != 0xff) {
					data->ssync.port_close();
					data->ssync.port_open(inb[0]);
					clear();
				}
				disp[43] = "";
				kbuf->clear();
			}
			break;
		case 20:
			clear();
			break;
		case 30:
			disp[43] = kbuf->fullbuf();
			if ( kbuf->enter() ) {
				ini = str2int(kbuf->fullbuf(), 0x80000000);
				if (ini != 0x80000000) {
					data->start_delay = ini;
				}
				kbuf->clear();
				clearStrings(0,43);
				disp[42] = "Enter start->stop (off->on) delay in milliseconds:";
				minor = 31;
			}
			break;
		case 31:
			disp[43] = kbuf->fullbuf();
			if ( kbuf->enter() ) {
				ini = str2int(kbuf->fullbuf(), 0x80000000);
				if (ini != 0x80000000) {
					data->stop_delay = ini;
				}
				kbuf->clear();
				clear();
			}
			break;

	}
}

void HockeyLogic::logicQuit(HockeyData* data, HockeyDraw* hd, Keybuffer* kbuf, HockeyDrop* drop) {
	int key;
	switch (minor) {
		case 0:
			clearStrings(0,43);
			disp[42] = "Are you sure you want to quit? Press Y to quit.";
			minor = 1;
			kbuf->clear();
			break;
		case 1:
			key = kbuf->last();
			if (key == 'y' || key == 'Y') {
				data->allow_quit = true;
				return;
			}
			kbuf->clear();
			clear();
			break;
	}
}
#endif
