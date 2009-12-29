#ifndef _hockeydraw_h_
#define _hockeydraw_h_

#include <windows.h>
#include <gl/gl.h>
#include <gl/glu.h>
#include "HockeyData.h"
#include "HockeyDrop.h"

inline void get_clock_parts(Mclock& clk, int& min, int& sec, int& tenths) {
	int ms = clk.read();
	if ( ms % 100 == 0 ) tenths = ms / 100;
	else tenths = (ms + 100) / 100;
	min = tenths / (60 * 10);
	sec = (tenths / 10) % 60;
	tenths %= 10;
}

class HockeyDraw {
public:
	int offset;			// shift of bar from initial position in y

// surfaces

	// pngs
	GLCairoSurface* redscore;
	GLCairoSurface* yellowname;
	GLCairoSurface* team_mask;
	GLCairoSurface* team_gloss;

	// text
	GLCairoTextSurface* score[2];
	GLCairoTextSurface* name[2];
	GLCairoTextSurface* dark_name[2];
	GLCairoTextSurface* clock;
	GLCairoTextSurface* period;

	// mix
	GLCairoSurface* ync[2];
	GLCairoSurface* bg;
	GLCairoSurface* main;

	HockeyDraw();
	~HockeyDraw();
	void init_surfaces();
	void draw(HockeyData* data, HockeyDrop* drop);
	void update_team_color(int team, int r, int g, int b);
};

bool drawRect(int xl, int xh, int yl, int yh, int z);


#endif