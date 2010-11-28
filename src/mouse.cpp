#include "main.h"

// mouse stuff
#ifdef WINDOWS
HCURSOR curN, curG, curB;
#endif

void mousemove(int x, int y) {
	unsigned char& key = data->keymode;
#ifdef WINDOWS
	if (key == 'g') SetCursor(curG);
	else if (key == 'b') SetCursor(curB);
	else SetCursor(curN);

	if ( y >= 0 && y < 200 || key == 'x' ) {
		SetCursor(NULL);
	}
#endif

}

void mouseclick(int button, int state, int x, int y) {
	unsigned char& key = data->keymode;
#ifdef WINDOWS
	if (key == 'g') SetCursor(curG);
	else if (key == 'b') SetCursor(curB);
	else SetCursor(curN);

	if ( y >= 0 && y < 200 || key == 'x' ) {
		SetCursor(NULL);
	}
#endif

	// future mouse functionality
	//if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
	//	get_mouse(x,y);
	//}

}
