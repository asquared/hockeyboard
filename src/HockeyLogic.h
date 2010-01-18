#ifndef _hockeylogic_h_
#define _hockeylogic_h_

#include <string>
#include <sstream>
#include "HockeyData.h"
#include "HockeyDraw.h"
#include "HockeyDrop.h"
#include "Keybuffer.h"

using std::string;
using std::stringstream;

class HockeyLogic {
	typedef void (HockeyLogic::*guifunc)(HockeyData*, HockeyDraw*, Keybuffer*, HockeyDrop*);

private:
	static const char* penalties[25];
	static const char* gann[6];
	unsigned char inb[4];		// variables that save the user's inputs.  inb[0] is usually the team.
	int ini;					// variable that saves the user's input.  usually a clock value.
	string input;

	unsigned char major;
	unsigned char minor;

	void logicClear(HockeyData* data, HockeyDraw* hd, Keybuffer* kbuf, HockeyDrop* drop);

	void logicClock(HockeyData* data, HockeyDraw* hd, Keybuffer* kbuf, HockeyDrop* drop);
	void logicIntClk(HockeyData* data, HockeyDraw* hd, Keybuffer* kbuf, HockeyDrop* drop);
	void logicGoals(HockeyData* data, HockeyDraw* hd, Keybuffer* kbuf, HockeyDrop* drop);
	void logicGAnn(HockeyData* data, HockeyDraw* hd, Keybuffer* kbuf, HockeyDrop* drop);
	void logicKey(HockeyData* data, HockeyDraw* hd, Keybuffer* kbuf, HockeyDrop* drop);
	void logicNames(HockeyData* data, HockeyDraw* hd, Keybuffer* kbuf, HockeyDrop* drop);
	void logicColors(HockeyData* data, HockeyDraw* hd, Keybuffer* kbuf, HockeyDrop* drop);
	void logicRoster(HockeyData* data, HockeyDraw* hd, Keybuffer* kbuf, HockeyDrop* drop);
	void logicPAnn(HockeyData* data, HockeyDraw* hd, Keybuffer* kbuf, HockeyDrop* drop);
	void logicPTime(HockeyData* data, HockeyDraw* hd, Keybuffer* kbuf, HockeyDrop* drop);
	void logicPeriod(HockeyData* data, HockeyDraw* hd, Keybuffer* kbuf, HockeyDrop* drop);
	void logicReset(HockeyData* data, HockeyDraw* hd, Keybuffer* kbuf, HockeyDrop* drop);
	void logicText(HockeyData* data, HockeyDraw* hd, Keybuffer* kbuf, HockeyDrop* drop);
	void logicStat(HockeyData* data, HockeyDraw* hd, Keybuffer* kbuf, HockeyDrop* drop);
	void logicSync(HockeyData* data, HockeyDraw* hd, Keybuffer* kbuf, HockeyDrop* drop);
	void logicQuit(HockeyData* data, HockeyDraw* hd, Keybuffer* kbuf, HockeyDrop* drop);

	guifunc funcarray[58];

public:
	string disp[44];
	unsigned char next;

	HockeyLogic();

	void clearStrings(int, int);

	void clear();
	bool isClear() { return major == 0; }

	void selectMajor(HockeyData* data, HockeyDraw* hd, Keybuffer* kbuf, HockeyDrop* drop);

};


#endif