#ifndef _main_h_
#define _main_h_

// required libraries
#pragma comment(lib, "opengl32")
#pragma comment(lib, "glu32")
#pragma comment(lib, "glaux")

#include <windows.h>		// Header File For Windows
#include <math.h>			// Header File For Windows Math Library
#include <stdio.h>			// Header File For Standard Input/Output
#include <stdarg.h>			// Header File For Variable Argument Routines

#include <gl/gl.h>
#include <gl/glu.h>
#include <gl/glut32w.h>

#include "..\vc7\resource.h"
#include "FreeType.h"
#include "HockeyData.h"
#include "HockeyDrop.h"
#include "HockeyLogic.h"
#include "Keybuffer.h"
#include "HockeyDraw.h"

// global pointers
extern freetype::font_data* base;
extern HockeyData* data;
extern HockeyLogic* logic;
extern HockeyDrop* drop;
extern HockeyDraw* hd;
extern Keybuffer* kbuf;
extern HCURSOR curN, curG, curB;

// callbacks
void display();
void timer(int parm);
void keyboard(unsigned char key, int x, int y);
void specialkey( int key, int x, int y );
void mousemove(int x, int y);
void mouseclick(int button, int state, int x, int y);

void make2d();
void quit(int code);

#endif
