#ifndef _main_h_
#define _main_h_

#ifdef WINDOWS
// required libraries
#pragma comment(lib, "opengl32")
#pragma comment(lib, "glu32")
#pragma comment(lib, "glaux")

#include <windows.h>		// Header File For Windows
#endif

#include <math.h>			// Header File For C Math Library
#include <stdio.h>			// Header File For Standard Input/Output
#include <stdarg.h>			// Header File For Variable Argument Routines

#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>

//#include "..\vc7\resource.h"
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

#ifdef WINDOWS
extern HCURSOR curN, curG, curB;
#endif

// callbacks
void display();
void timer(int parm);
void keyboard(unsigned char key, int x, int y);
void specialkey( int key, int x, int y );
void mousemove(int x, int y);
void mouseclick(int button, int state, int x, int y);

void load_settings(const char* filename);
void make2d();
void quit(int code);

#endif
