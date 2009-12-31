#include "main.h"

// global pointers
freetype::font_data* base;
HockeyData* data;
HockeyLogic* logic;
HockeyDrop* drop;
HockeyDraw* hd;
Keybuffer* kbuf;

/* // framerate stuff
Mclock* clk;
int framect = 0;
int lastframect = 0;
int lastms = 0;
int sleeptime = 10;
*/

void display() {
	if (data->allow_quit) {
		quit(0);
	}

	/* // framerate counter
	++framect;

	clk->update();
	int ms = clk->read();

	if (lastms + 1000 < ms) {
		lastframect = framect;
		framect = 0;
		lastms += 1000;
		if (lastframect >= 58) sleeptime++;
		else if (lastframect < 58) sleeptime--;
		if (sleeptime < 0) sleeptime = 0;
		else if (sleeptime > 100) sleeptime = 100;
		std::cout << lastframect << " " << sleeptime << std::endl;
	}
	*/ // end framerate counter


	HockeyData& d = *data;
	float x, y;
	
	unsigned char key = data->keymode;
	if (key == 'b') {
		glClearColor(0.0f, 0.0f, 240.0f/255.0f, 1.0f);
		glColor3ub(0,0,180);
	}
	else if (key == 'g') {
		glClearColor(0.0f, 240.0f/255.0f, 0.0f, 1.0f);
		glColor3ub(0,180,0);
	}
	else if (key == 'x') {
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glColor3ub(255,255,255);
	}
	else {
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glColor3ub(180,180,180);
	}
	glClear(GL_COLOR_BUFFER_BIT);

	if ( key == 'x' ) {
		drawRect(0.0f, 1024.0f, 680.0f, 728.0f, 0.0f);
		glutSwapBuffers();
		return;
	}

	// synchronizer
	d.do_sync();

	// clock stuff
	d.clock.update();
	if (!d.clock.running()) d.clock_last_stopped = d.clock.read();
	d.int_clock.update();
	d.red_flash_clock.update();
	int tenths, sec, min;
	get_clock_parts(d.clock, min, sec, tenths);
	base->print(740, 500, 0, "Game:   %02d:%02d.%01d", min, sec, tenths);
	if (d.sync) base->print(940, 460, 0, "SYNC %c", d.sstat);
	base->print(940, 500, 0, "P:%3hhu", d.period);
	get_clock_parts(d.int_clock, min, sec, tenths);
	base->print(740, 460, 0, "Int.:   %02d:%02d.%01d", min, sec, tenths);
	if (d.active_clock == &(d.int_clock)) base->print(720, 460, 0, "\x10");
	else base->print(720, 500, 0, "\x10");
	
	// teams and scores
	base->print(170, 500, 0, "%-5s %3hhu", d.name[0].substr(0,5).c_str(), d.sc[0]);
	base->print(170, 480, 0, "%-5s %3hhu", d.name[1].substr(0,5).c_str(), d.sc[1]);

	// drop text strings (8-39)
	signed short curr_state = drop->user_state;
	base->print(360, 500, 0, "\x10");
	for (int i = 0; i <= 3; ++i)
		if (curr_state + i < MAX_STATE)
			base->print(380, 500-20*i, 0, "%2hhd: %s", curr_state+i, drop->getstring(curr_state+i).substr(0,30).c_str());

	// penalty timers
	data->printPenalties(logic->disp, base);

	// UI strings (0-41)
	for ( int i = 0; i < 42; ++i ) {
		x = (i / 14) * 340 + 10;
		y = 320 - (i % 14) * 20;
		base->print(x, y, 0, logic->disp[i].c_str());
	}

	// blinking cursor
	int blink_time = 0;
	if (!logic->isClear()) {
		blink_time = (timeGetTime() >> 9) & 1;
	}

	//base->print(0, 60, 0, "\x10\x11\x12\x13\x14\x15\x16\x17\x18\x19\x1a\x1b\x1c\x1d\x1e\x1f");
	base->print(0, 40, 0, logic->disp[42].c_str());
	if (!blink_time) base->print(0, 20, 0, logic->disp[43].c_str());
	else base->print(0, 20, 0, (logic->disp[43] + "\x15").c_str());

	// powerplay stuff for drops
	unsigned short vis, home, psec, pmin;
	d.getPenaltyInfo(vis, home, pmin, psec);
	//base->print(20, 400, 0, "%-5s %hu", d.name[0].substr(0,5).c_str(), vis );
	//base->print(20, 380, 0, "%-5s %hu", d.name[1].substr(0,5).c_str(), home);
	if (vis-1 > home) base->print(130, 500, 0, "++");
	else if (vis > home) base->print(140, 500, 0, "+");
	else if (home-1 > vis) base->print(130, 480, 0, "++");
	else if (home > vis) base->print(140, 480, 0, "+");
	base->print(100, 400, 0, "%hu on %hu  --", vis, home);
	base->print(210, 400, 0, "%2hu:%02hu", pmin, psec);
	drop->ppdata(vis-home, min(vis, home), pmin, psec);

	// bitmaps

	// stupid yellow team names
	bool yellow_l = (drop->getyellow() == 1);
	bool yellow_r = (drop->getyellow() == 2);

	hd->draw(data, drop);

	if (key == 'b' || key == 'g') {
		if (key == 'b') glColor3ub(0, 0, 240);
		else glColor3ub(0, 240, 0);
		int y = hd->offset;
		drawRect(0, 1024, 728+y, 736+y, 0);
		drawRect(0, 1024, 672+y, 680+y, 0);
	}

	glutSwapBuffers();
}

void timer(int parm) {
	glutPostRedisplay();
	glutTimerFunc( 1000/60, timer, 1 );
}



void make2d() {
	int oldmode;
	glGetIntegerv(GL_MATRIX_MODE, &oldmode);
	glPushMatrix();
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	//glOrtho(0.0f, 1400.0f, 0.0f, 1050.0f, 0.0f, 1.0f);
	glOrtho(0.0f, 1280.0f, 0.0f, 1024.0f, 0.0f, 1.0f);
	//glOrtho(0.0f, 1024.0f, 0.0f, 768.0f, 0.0f, 1.0f);
	glPopMatrix();
	glMatrixMode(oldmode);
}

int main( int argc, char* argv[] ) {

	// initialize GLUT
	int argcp = 1;
    glutInit( &argcp , &(argv[0]) );

    // set RGBA mode with double and depth buffers
    glutInitDisplayMode( GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH );

	//glutGameModeString( "1400x1050:32@60" );
	glutGameModeString( "1280x1024:32@60" );
	//glutGameModeString( "1024x768:32@60" );
    // start fullscreen game mode
    glutEnterGameMode();

	make2d();

	// load fonts
	base = new freetype::font_data;
	try {
		string fontdir = string(getenv("WINDIR"));
		fontdir += "\\Fonts\\";
		//base->init((fontdir + "verdana.ttf").c_str(), 12, 16, 255);
		base->init(".\\DejaVuLGCSansMono.ttf", 12, 16, 255);
	}
	catch (std::runtime_error e) {
		quit(1);
	}

	// generate data object
	data = new HockeyData;
	logic = new HockeyLogic;
	kbuf = new Keybuffer;
	drop = new HockeyDrop(428.0f, 676.0f);

	/* // framerate clock
	clk = new Mclock();
	clk->start();
	*/

	// mouse cursors
	POINT p;
	GetCursorPos(&p);
	if ( p.y >= 0 && p.y < 100 ) { 
		SetCursor(NULL);
	}
	HINSTANCE hInstance = (HINSTANCE) glutGetHINSTANCE();
	curN = LoadCursor(NULL, IDC_ARROW);
	curG = LoadCursor(hInstance, MAKEINTRESOURCE(IDC_GREEN));
	curB = LoadCursor(hInstance, MAKEINTRESOURCE(IDC_BLUE));
	
	// display starting text
	logic->clear();

	// new draw code
	hd = new HockeyDraw;

	// setup window callbacks
    glutDisplayFunc( display );
    glutKeyboardFunc( keyboard );
    glutTimerFunc( 1000/60, timer, 1 );
	glutSpecialFunc( specialkey );
	glutMotionFunc( mousemove );
	glutPassiveMotionFunc( mousemove );
	glutMouseFunc( mouseclick );


	// enter main loop
    glutMainLoop();

	quit(0);
	return 0;
}


void quit(int code) {
	delete base;
	delete drop;
	delete data;
	delete logic;
	delete kbuf;
	delete hd;
	exit(code);
}	