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
const int MAX_LINES = 48;
const int MAX_STATE = MAX_LINES;
const int MIN_USER_STATE = 8;

class HockeyDrop {
private:
	void set_lines();
	void load_graphics();

	string lines[MAX_LINES];
	short min, sec;
	float x, y;
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
	HockeyDrop(float xin, float yin);
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