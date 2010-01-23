#include "HockeyDrop.h"

std::string gettime(short min, short sec) {
	char c[20];
	if (min != 0) sprintf(c, "%1hd:%02hd", min, sec);
	else sprintf(c, ":%02hd", sec);
	return std::string(c);
}
inline void set_font_defaults_black(GLCairoSurface* glcs, int size) {
	//glcs->setfontface("Franklin Gothic Demi Cond", false, false, false, 54);
	//glcs->setfontfacealt(0, "Franklin Gothic Demi Comp", false, false, false, 54);
	glcs->setfontface("Gotham Narrow Bold", false, false, false, 25);
	glcs->setfontfacealt(0, "Gotham XNarrow Bold", false, false, false, 25);
	//glcs->setfontface("DIN Next LT Pro Bold", false, false, false);
	//glcs->setfontfacealt(0, "DIN Next LT Pro Bold Condensed", false, false, false);
	glcs->setfontsize(size);
	glcs->color(19,19,19);
}

void HockeyDrop::load_graphics() {
	try {
		base_y = new GLCairoSurface("hb3_yellow.png");
		base_w = new GLCairoSurface("hb3_white.png");
		pp_y = new GLCairoSurface("hb3_pp_yellow.png");
		pp_w = new GLCairoSurface("hb3_pp_white.png");
	}
	catch (int) {
		std::cout << "Error: PNG file not found or read error\n";
	}
	text = new GLCairoTextSurface(347, 36);
	set_font_defaults_black(text, 28);
	strength = new GLCairoTextSurface(248, 32);
	set_font_defaults_black(strength, 28);	
	pptime = new GLCairoTextSurface(112, 36);
	set_font_defaults_black(pptime, 32);
	pptime->setfontface("Gotham FWN Narrow Bold", false, false, false, 25);
	compf = new GLCairoSurface(355, 40);
	compb = new GLCairoSurface(355, 40);
	comp = new GLCairoSurface(355, 40);
}

void HockeyDrop::set_lines() {
	lines[SI_DP] = string("DELAYED PENALTY");
	lines[SI_EN_V] = lines[SI_EN_H] = string("EMPTY NET");
	lines[SI_PS_V] = lines[SI_PS_H] = string("PENALTY SHOT");
	lines[SI_TO_V] = lines[SI_TO_H] = string("TIMEOUT");
}

HockeyDrop::HockeyDrop() {
	x = y = 0;
	old_yellow = -1;
	user_state = MIN_USER_STATE;
	state = -1;
	ppstate = 0;
	moving = false;
	load_graphics();
	set_lines();
}

HockeyDrop::HockeyDrop(int xin, int yin) {
	x = xin;
	y = yin;
	old_yellow = -1;
	user_state = MIN_USER_STATE;
	state = -1;
	ppstate = 0;
	moving = false;
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
	if (compf) delete compf;
	if (compb) delete compb;
	if (comp) delete comp;
}

void HockeyDrop::set_state(unsigned char type) {
	if ( type >= MIN_USER_STATE && type <= MAX_USER_STATE ) 
		user_state = type;
	if ( state >= MIN_USER_STATE && state <= MAX_USER_STATE ) 
		drop(user_state);
}

void HockeyDrop::drop(unsigned char type) {
	if ( !moving && type >= 0 && type < MAX_LINES && state != type ) {
		old_yellow = getyellow();
		state = type;
		droptime.set(0);
		droptime.start();
		moving = true;
		std::swap(compf, compb);
	}
}

void HockeyDrop::raise() {
	if ( !moving && state != -1 ) {
		old_yellow = getyellow();
		state = -1;
		droptime.set(0);
		droptime.start();
		moving = true;
		std::swap(compf, compb);
	}
}

void HockeyDrop::toggle(unsigned char type) {
	if ( type >= 0 && type < MAX_LINES ) {
		if ( state == type ) raise();
		else drop(type);
	}
}

void HockeyDrop::setstring(unsigned char type, const string& str) {
	if ( type >= 0 && type < MAX_LINES ) lines[type] = str;
}

string HockeyDrop::getstring(signed short line) {
	if (line >= 0 && line < MAX_LINES) return lines[line];
	return string("");
}

char HockeyDrop::getid(unsigned char line) {
	if (line < 30) return ' ';
    else if (line <= 41) return 'P';
	else if (line <= 49) return 'G';
	else return ' ';
}

void HockeyDrop::settime(short min_in, short sec_in) {
	min = min_in;
	sec = sec_in;
}

float HockeyDrop::display(GLCairoSurface* main) {
	int time;			// time count in msec
	float timef;		// time count in sec

	if ( moving ) {
		droptime.update();
		time = droptime.read();
		timef = ((float) time) / 1000;
		if (timef >= TK || timef < 0.0f) {
			alpha = 1.0f;
			moving = false;
			droptime.stop();
		}
		else {
			alpha = timef / TK;
		}
	}

	// user_state sync
	if ( state >= MIN_USER_STATE && state <= MAX_USER_STATE && 
		user_state >= MIN_USER_STATE && user_state <= MAX_USER_STATE ) 
		state = user_state;

	//glPixelTransferf(GL_ALPHA_SCALE, alpha);
	compf->clear();

	switch (state) {
		case SI_PP:
		case SI_PP_EN:
			// power play (+ EMPTY NET)
			if (ppstate >= 1) {
				pp_y->painton(compf, 0, 0, 1.0);
			}
			else if (ppstate == -1) {
				base_w->painton(compf, 0, 0, 1.0);
			}
			else { 
				pp_w->painton(compf, 0, 0, 1.0);
			}
			if (ppstate >= 0) {
				//strength->writetext(lines[SI_PP], 14, -3, 0);
				strength->writetextshrink(lines[state], 14, 16, 0, 226);
				pptime->writetext(gettime(min, sec), 97, -9, 2);
				strength->painton(compf, 5, 4, 1.0);
				pptime->painton(compf, 239, 2, 1.0);
			}
			else {
				text->writetextshrink(lines[state], 173, 18, 1, 322);
				text->painton(compf, 4, 2, 1.0);
			}
			break;
		case SI_EN_V:
		case SI_EN_H:
		case SI_PS_V:
		case SI_PS_H:
		case SI_DP:
			// yellow lines (EMPTY NET, PENALTY SHOT, DELAYED PENALTY)
			base_y->painton(compf, 0, 0, 1.0);
			text->writetextshrink(lines[state], 173, 18, 1, 322);
			text->painton(compf, 4, 2, 1.0);
			break;
		case SI_TO_V:
		case SI_TO_H:
			// white lines (TIMEOUT)
			base_w->painton(compf, 0, 0, 1.0);
			text->writetextshrink(lines[state], 173, 18, 1, 322);
			text->painton(compf, 4, 2, 1.0);
			break;
		case -1:
			// do nothing
			break;
		default:
			// white lines (custom)
			if (state >= 0 && state <= MAX_USER_STATE) {
				base_w->painton(compf, 0, 0, 1.0);
				text->writetextshrink(lines[state], 173, 18, 1, 322);
				text->painton(compf, 4, 2, 1.0);
			}
			else {
				state = -1;
				raise();
			}
	}

	//compb->painton(main, x, y, 1.0f-alpha);
	//compf->painton(main, x, y, alpha);
	if (alpha != 1.0f) {
		comp->mixfrom(compf, compb, alpha);
		comp->painton(main, x, y, 1.0f);
	}
	else {
		compf->painton(main, x, y, 1.0f);
	}

	return alpha;
	//glPixelTransferf(GL_ALPHA_SCALE, 1.0f);
}

// ppstate:  -1: full strength  0: even strength  1: vis power play  2: home power play
void HockeyDrop::ppdata(short adv, unsigned short strength, unsigned short pmin, unsigned short psec) {
	if ( adv == 0 ) {
		if (strength == 5) {
			lines[SI_PP] = "FULL STRENGTH";
			lines[SI_PP_EN] = "EMPTY NET";
			// automatically change to the normal (yellow) EMPTY NET graphic when 
			// the power play expires (assumes same team has PP and EN simultaneously)
			if (state == SI_PP_EN && ppstate > 0) state = SI_EN_V - 1 + ppstate;
			settime(0,0);
			ppstate = -1;
		}
		else {
			if (strength == 4) {
				lines[SI_PP] = "4-ON-4";
				lines[SI_PP_EN] = "4-ON-4 + EMPTY NET";
			}
			else if (strength == 3) {
				lines[SI_PP] = "3-ON-3"; 
				lines[SI_PP_EN] = "3-ON-3 + EMPTY NET";
			}
			settime(pmin, psec);
			ppstate = 0;
		}
	}
	else if ( abs(adv) == 1 ) {
		if (strength == 3) {
			lines[SI_PP] = "4-ON-3";
			lines[SI_PP_EN] = "4-ON-3 + EMPTY NET";
		}
		else {
			lines[SI_PP] = "POWER PLAY";
			lines[SI_PP_EN] = "PP + EMPTY NET";
		}
		settime(pmin, psec);
		if (adv > 0) ppstate = 1;
		else ppstate = 2;
	}
	else if ( abs(adv) == 2 ) {
		lines[SI_PP] = "2-MAN ADV";
		lines[SI_PP_EN] = "2MA + EMPTY NET";
		settime(pmin, psec);
		if (adv > 0) ppstate = 1;
		else ppstate = 2;
	}
	else state = -1;   // invalid case
}

// return codes:
// -1: no yellow  0: visitor yellow  1: home yellow
signed char HockeyDrop::getyellow() {
	if ( state <= MAX_USER_STATE || state >= SI_DP || 
		state == SI_TO_V || state == SI_TO_H ) return -1;
	if ( state == SI_EN_V || state == SI_PS_V ) return 0;
	if ( state == SI_EN_H || state == SI_PS_H ) return 1;
	else return ppstate-1;
}

void HockeyDrop::getyellow(float y[2]) {
	y[0] = y[1] = 0.0f;
	signed char yellow = getyellow();
	if (old_yellow >= 0) y[old_yellow] += (1.0f - alpha);
	if (yellow >= 0) {
		y[yellow] += alpha;
		if (yellow == old_yellow) y[yellow] = 1.0f;
	}
}

/*
formulae:
t = half of the time for full drop
h = height to drop
x = current time
0 < x < t : 1/2 * h/t^2 * x^2
t < x < 2t : -1/2 * h/t^2 * (x-2t)^2 + h
*/