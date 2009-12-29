#include "main.h"

// mouse stuff
HCURSOR curN, curG, curB;

void mousemove(int x, int y) {
	unsigned char& key = data->keymode;
	if (key == 'g') SetCursor(curG);
	else if (key == 'b') SetCursor(curB);
	else SetCursor(curN);

	if ( y >= 0 && y < 200 || key == 'x' ) {
		SetCursor(NULL);
	}

}

void mouseclick(int button, int state, int x, int y) {
	unsigned char& key = data->keymode;
	if (key == 'g') SetCursor(curG);
	else if (key == 'b') SetCursor(curB);
	else SetCursor(curN);

	if ( y >= 0 && y < 200 || key == 'x' ) {
		SetCursor(NULL);
	}

	// future mouse functionality
	//if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
	//	get_mouse(x,y);
	//}

}