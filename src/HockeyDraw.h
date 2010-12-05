#ifndef _hockeydraw_h_
#define _hockeydraw_h_

/* must go here so winsock2.h is included before windows.h... ugh */
#include "ClockSocket.h" 

#ifdef WINDOWS
#include <windows.h>
#endif

#include <GL/gl.h>
#include <GL/glu.h>
#include "HockeyData.h"
#include "HockeyDrop.h"


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

	// clock output (to instant replay etc...)
	ClockSocket clock_socket;

	HockeyDraw();
	~HockeyDraw();
	void init_surfaces();
	void draw(HockeyData* data, HockeyDrop* drop);
	void update_team_color(int team, int r, int g, int b);
};

bool drawRect(int xl, int xh, int yl, int yh, int z);


#endif
