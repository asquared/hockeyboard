#ifndef _hockeydrop_h_
#define _hockeydrop_h_
#include <string>
//#include "GLBitmap.h"
#include "GLCairoTextSurface.h"
#include "FreeType.h"
#include "Mclock.h"

using std::string;

// constants for timing
const static float HT[3] = { 35.0f, 35.0f, 50.0f };
const static float TK = .1f;

// constant for number of lines/states
const int MAX_LINES = 57;
const int MIN_USER_STATE = 0;
const int MAX_USER_STATE = 47;

// constants for special states
const int SI_EN_V = 48;		// EMPTY NET
const int SI_EN_H = 49;
const int SI_PS_V = 50;		// PENALTY SHOT
const int SI_PS_H = 51;
const int SI_TO_V = 52;		// TIMEOUT
const int SI_TO_H = 53;
const int SI_PP = 54;		// POWER PLAY
const int SI_PP_EN = 55;	// PP + EMPTY NET
const int SI_DP = 56;		// DELAYED PENALTY

class HockeyDrop {
private:
	void set_lines();
	void load_graphics();

	string lines[MAX_LINES];
	short min, sec;
	int x, y;
	bool moving;
	bool down;
	Mclock droptime;

	GLCairoSurface* base_y, * base_w, * pp_y, * pp_w;
	GLCairoTextSurface* text, * strength, * pptime;
	GLCairoSurface* composite;

public:
	signed char user_state;
	signed char state;
	signed char ppstate;
	
	HockeyDrop();
	HockeyDrop(int xin, int yin);
	~HockeyDrop();
	void loadBitmap(unsigned char type, GLCairoSurface* bitmap);
	void drop(unsigned char type);
	void raise();
	void toggle(unsigned char type);
	void setstring(unsigned char type, string& str);
	string getstring(signed short line);
	void settime(short min_in, short sec_in);
	float display(GLCairoSurface* main);
	void ppdata( short adv, unsigned short strength, unsigned short pmin, unsigned short psec);
	short getyellow();

};

#endif