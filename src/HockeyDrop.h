#ifndef _hockeydrop_h_
#define _hockeydrop_h_
#include <string>
//#include "GLBitmap.h"

#include "HockeyData.h"
#include "GLCairoTextSurface.h"
#include "FreeType.h"
#include "Mclock.h"

using std::string;

// constant for number of lines/states
const int MAX_LINES = 69;
const int MIN_USER_STATE = 0;
const int MAX_USER_STATE = 59;
const int MAX_WRITE_STATE = 57;

// constants for special states
const int SI_EN_V = 60;		// EMPTY NET
const int SI_EN_H = 61;
const int SI_PS_V = 62;		// PENALTY SHOT
const int SI_PS_H = 63;
const int SI_TO_V = 64;		// TIMEOUT
const int SI_TO_H = 65;
const int SI_PP = 66;		// POWER PLAY
const int SI_PP_EN = 67;	// PP + EMPTY NET
const int SI_DP = 68;		// DELAYED PENALTY
const int SI_SOG = 59;		// SHOTS ON GOAL (stat)

class HockeyDrop {
private:
	void load_graphics();
	void common_constructor();

	string lines[MAX_LINES];	// text for each state
	short min, sec;				// power play clock
	int x, y;					// position of this element
	float alpha;				// alpha value for fading
	Mclock droptime;			// fade timer
	bool moving;				// is element currently fading?

	GLCairoSurface* base_y, * base_w, * base_r, * pp_y, * pp_w;
	GLCairoTextSurface* text, * text_w, * strength, * pptime;
	GLCairoSurface* stat_sog;
	GLCairoTextSurface* team[2], * val[2];
	GLCairoSurface* compf, * compb, * comp;

public:
	signed char old_yellow;		// yellow state for the fading out state
	signed char user_state;		// state between 0-47 currently pointed at
	signed char state;			// state currently displayed (-1 = nothing)
	signed char ppstate;		// power play status
	float TK;					// fade time
	
	HockeyDrop();
	HockeyDrop(int xin, int yin);
	~HockeyDrop();
	void set_state(unsigned char type);
	void drop(unsigned char type);
	void raise();
	void toggle(unsigned char type);
	void setstring(unsigned char type, const string& str);
	char getid(unsigned char line);
	string getstring(signed short line);
	void settime(short min_in, short sec_in);
	float display(GLCairoSurface* main, HockeyData* data);
	void ppdata(short adv, unsigned short strength, unsigned short pmin, unsigned short psec);
	void draw_if(freetype::font_data* base);
	signed char getyellow();
	void getyellow(float y[2]);

};

#endif