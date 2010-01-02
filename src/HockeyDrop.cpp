#include "HockeyDrop.h"

std::string gettime(short min, short sec) {
	char c[20];
	if (min != 0) sprintf(c, "%1hd:%02hd", min, sec);
	else sprintf(c, ":%02hd", sec);
	return std::string(c);
}
inline void set_font_defaults_black(GLCairoSurface* glcs, int size) {
	glcs->setfontface("Franklin Gothic Demi Cond", false, false, false);
	glcs->setfontfacealt(0, "Franklin Gothic Demi Comp", false, false, false);
	//glcs->setfontface("Gotham Narrow Bold", false, false, false);
	//glcs->setfontfacealt(0, "Gotham XNarrow Bold", false, false, false);
	glcs->setfontsize(size);
	glcs->color(19,19,19);
}

void HockeyDrop::load_graphics() {
	try {
		base_y = new GLCairoSurface("hb_yellow.png");
		base_w = new GLCairoSurface("hb_white.png");
		pp_y = new GLCairoSurface("hb_pp_yellow.png");
		pp_w = new GLCairoSurface("hb_pp_white.png");
	}
	catch (int) {
		std::cout << "Error: PNG file not found or read error\n";
	}
	text = new GLCairoTextSurface(335, 44);
	set_font_defaults_black(text, 30);
	strength = new GLCairoTextSurface(220, 44);
	set_font_defaults_black(strength, 30);	
	pptime = new GLCairoTextSurface(123, 44);
	set_font_defaults_black(pptime, 36);
	composite = new GLCairoSurface(335, 44);
}

void HockeyDrop::set_lines() {
	lines[1] = string("DELAYED PENALTY");
	lines[2] = lines[3] = string("EMPTY NET");
	lines[4] = lines[5] = string("PENALTY SHOT");
	lines[6] = lines[7] = string("TIMEOUT");
}

HockeyDrop::HockeyDrop() {
	x = y = 0;
	user_state = 8;
	state = -1;
	ppstate = 0;
	moving = down = false;
	load_graphics();
	set_lines();
}

HockeyDrop::HockeyDrop(float xin, float yin) {
	x = xin;
	y = yin;
	user_state = 8;
	state = -1;
	ppstate = 0;
	moving = down = false;
	load_graphics();
	set_lines();
}

HockeyDrop::~HockeyDrop() {
	if (base_y) delete base_y;
	if (base_w) delete base_w;
	if (pp_y) delete pp_y;
	if (pp_w) delete pp_w;
	if (text) delete text;
	if (strength) delete strength;
	if (pptime) delete pptime;
	if (composite) delete composite;
}

void HockeyDrop::loadBitmap(unsigned char type, GLCairoSurface* bitmap) {
	switch (type) {
		case 0:
			base_y = bitmap;
			break;
		case 1:
			base_w = bitmap;
			break;
		case 2:
			pp_y = bitmap;
			break;
		case 3:
			pp_w = bitmap;
			break;
		default:
			break;
	}
}

void HockeyDrop::drop(unsigned char type) {
	if ( !moving && type >= 0 && type < MAX_LINES ) {
		if (state == -1) {
			state = type;
			droptime.set(0);
			droptime.start();
			moving = true;
			down = true;
		}
		else state = type;
	}
}

void HockeyDrop::raise() {
	if ( !moving ) {
		droptime.set(0);
		droptime.start();
		moving = true;
		down = false;
	}
}

void HockeyDrop::toggle(unsigned char type) {
	if ( state == -1 && type >= 0 && type < MAX_LINES ) drop(type);
	else if ( state >= 0 && state == type ) raise();
	else state = type;
}

void HockeyDrop::setstring(unsigned char type, string& str) {
	if ( type >= 0 && type < MAX_LINES ) lines[type] = str;
}

string HockeyDrop::getstring(signed short line) {
	if (line >= 0 && line < MAX_LINES) return lines[line];
	return string("");
}

void HockeyDrop::settime(short min_in, short sec_in) {
	min = min_in;
	sec = sec_in;
}

float HockeyDrop::display(GLCairoSurface* main) {
	float alpha = 1.0f;	// alpha value for fading
	int time;			// time count in msec
	float timef;		// time count in sec

	if ( moving ) {
		droptime.update();
		time = droptime.read();
		timef = ((float) time) / 1000;
		if (timef >= (TK) || timef < 0.0f) {
			moving = false;
			if ( !down ) state = -1;
			droptime.stop();
		}
		else if (timef >= 0.0f && timef <= TK) {
			alpha = timef / TK;
			if ( !down ) alpha = 1.0f - alpha;
		}
	}

	// user_state sync
	if ( state >= MIN_USER_STATE && state <= MAX_STATE && 
		user_state >= MIN_USER_STATE && user_state <= MAX_STATE ) 
		state = user_state;

	//glPixelTransferf(GL_ALPHA_SCALE, alpha);
	composite->clear();
	//text->clear();
	//strength->clear();
	//pptime->clear();
	//float len;
	switch (state) {
		// power play (lines[0])
		case 0:
			if (ppstate >= 1) {
				pp_y->painton(composite, 1, 0, 1.0);
			}
			else if (ppstate == -1) {
				base_w->painton(composite, 0, 0, 1.0);
			}
			else { 
				pp_w->painton(composite, 1, 0, 1.0);
			}
			if (ppstate >= 0) {
				strength->writetext(lines[0], 14, -3, 0);
				//strength->writetextshrink(lines[0], 14, 22, 0, 194);
				pptime->writetext(gettime(min, sec), 94, -8, 2);
				strength->painton(composite, 4, 0, 1.0);
				pptime->painton(composite, 220, 0, 1.0);
			}
			else {
				text->writetextshrink(lines[0], 167, 22, 1, 295);
				text->painton(composite, 0, 0, 1.0);
			}
			break;
		// yellow lines (lines[1 through 5])
		case 1:
		case 2:
		case 3:
		case 4:
		case 5:
			base_y->painton(composite, 0, 0, 1.0);
			text->writetextshrink(lines[state], 167, 22, 1, 295);
			text->painton(composite, 0, 0, 1.0);
			break;
		case 6:
		case 7:
			// TIMEOUT
			base_w->painton(composite, 0, 0, 1.0);
			text->writetextshrink(lines[state], 167, 22, 1, 295);
			text->painton(composite, 0, 0, 1.0);
			break;
		case -1:
			// do nothing
			break;
		// white lines (lines[8 through MAX_STATE-1])
		default:
			if (state >= 0 && state < MAX_STATE) {
				base_w->painton(composite, 0, 0, 1.0);
				text->writetextshrink(lines[state], 167, 22, 1, 295);
				text->painton(composite, 0, 0, 1.0);
			}
			else {
				state = -1;
				raise();
			}
	}
	composite->painton(main, 424, 10, alpha);

	return alpha;
	//glPixelTransferf(GL_ALPHA_SCALE, 1.0f);
}

// ppstate:  -1: full strength  0: even strength  1: vis power play  2: home power play
void HockeyDrop::ppdata( short adv, unsigned short strength, unsigned short pmin, unsigned short psec) {
	if ( adv == 0 ) {
		if (strength == 5) {
			lines[0] = "FULL STRENGTH";
			settime(0,0);
			ppstate = -1;
		}
		else {
			if (strength == 4) lines[0] = "4-ON-4";
			else if (strength == 3) lines[0] = "3-ON-3"; 
			settime(pmin, psec);
			ppstate = 0;
		}
	}
	else if ( abs(adv) == 1 ) {
		if (strength == 3) lines[0] = "4-ON-3";
		else lines[0] = "POWER PLAY";
		settime(pmin, psec);
		if (adv > 0) ppstate = 1;
		else ppstate = 2;
	}
	else if ( abs(adv) == 2 ) {
		lines[0] = "2-MAN ADV";
		settime(pmin, psec);
		if (adv > 0) ppstate = 1;
		else ppstate = 2;
	}
	else state = -1;   // invalid case
}

// return codes:
// 0: no yellow  1: visitor yellow  2: home yellow
short HockeyDrop::getyellow() {
	if ( state < 0 || state >= 6 || state == 1 ) return 0;
	if ( state == 2 || state == 4 ) return 1;
	if ( state == 3 || state == 5 ) return 2;
	else return ppstate;
}

/*
formulae:
t = half of the time for full drop
h = height to drop
x = current time
0 < x < t : 1/2 * h/t^2 * x^2
t < x < 2t : -1/2 * h/t^2 * (x-2t)^2 + h
*/