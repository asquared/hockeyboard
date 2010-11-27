#include "HockeyDraw.h"

inline void set_font_defaults(GLCairoSurface* glcs, int size) {
	//glcs->setfontface("Franklin Gothic Demi Cond", false, false, false, 54);
	glcs->setfontface("Gotham Narrow Bold", false, false, false, 25);
	//glcs->setfontface("DIN Next LT Pro Bold", false, false, false);
	glcs->setfontsize(size);
	glcs->color(240,240,240);
}

inline void set_font_alternate(GLCairoSurface* glcs, int size) {
	//glcs->setfontface("Franklin Gothic Demi Cond", false, false, false, 54);
	//glcs->setfontfacealt(0, "Franklin Gothic Demi Comp", false, false, false, 54);
	glcs->setfontface("Gotham Narrow Bold", false, false, false, 25);
	glcs->setfontfacealt(0, "Gotham XNarrow Bold", false, false, false, 25);
	//glcs->setfontface("DIN Next LT Pro Bold", false, false, false);
	//glcs->setfontfacealt(0, "DIN Next LT Pro Bold Condensed", false, false, false);
	glcs->setfontsize(size);
	glcs->color(240,240,240);
}

HockeyDraw::HockeyDraw() {
	offset = 0;
	init_surfaces();
}

void HockeyDraw::init_surfaces() {
	bg = 0;
	GLCairoSurface* team = 0, * clockper = 0;


	score[0] = new GLCairoTextSurface(77, 36);
	score[1] = new GLCairoTextSurface(77, 36);
	name[0] = new GLCairoTextSurface(114, 32);
	name[1] = new GLCairoTextSurface(114, 32);
	dark_name[0] = new GLCairoTextSurface(114, 32);
	dark_name[1] = new GLCairoTextSurface(114, 32);
	clock = new GLCairoTextSurface(133, 36);
	period = new GLCairoTextSurface(62, 36);
	main = new GLCairoSurface(1024, 64);
	ync[0] = new GLCairoSurface(114, 32);
	ync[1] = new GLCairoSurface(114, 32);

	try {
		bg = new GLCairoSurface("hb3_background.png");
		team = new GLCairoSurface("hb3_team.png");
		clockper = new GLCairoSurface("hb3_clock.png");
		redscore = new GLCairoSurface("hb3_redscore.png");
		yellowname = new GLCairoSurface("hb3_yellowteam.png");
		team_mask = new GLCairoSurface("hb3_teammask.png");
		team_gloss = new GLCairoSurface("hb3_teamgloss.png");
	}
	catch (int) {
		std::cout << "Error: PNG file not found or read error\n";
	}

	team->painton(bg, 70, 12, 1.0);
	team->painton(bg, 246, 12, 1.0);
	clockper->painton(bg, 766, 12, 1.0);

	set_font_alternate(clock, 32);
	clock->setfontface("Gotham FWN Narrow Bold", false, false, false, 25);
	set_font_defaults(period, 32);
	set_font_defaults(score[0], 32);
	set_font_defaults(score[1], 32);
	set_font_alternate(name[0], 28);
	set_font_alternate(name[1], 28);
	set_font_alternate(dark_name[0], 28);
	set_font_alternate(dark_name[1], 28);
	dark_name[0]->color(19,19,19);
	dark_name[1]->color(19,19,19);


	if (team) delete team;
	if (clockper) delete clockper;

}

HockeyDraw::~HockeyDraw() {
	delete score[0]; 
	delete score[1]; 
	delete name[0]; 
	delete name[1];
	delete clock, period, main;
	if (bg) delete bg;
}

void HockeyDraw::draw(HockeyData* data, HockeyDrop* drop) {
	// clock crap
	int min, sec, tenths;
	uint32_t clock_send;

	get_clock_parts(*(data->active_clock), min, sec, tenths);

	/* send clock data over socket to external systems (i.e. openreplay) */
	clock_send = htonl(min * 600 + sec * 10 + tenths);
	clock_socket.send(&clock_send, sizeof(clock_send));

	main->clear();
	bg->painton(main, 0, 0, 1.0);

	// red flash
	if (data->getRedFlash()) {
		redscore->painton(main, 170+176*(data->red_flash_team), 12, 1.0);
	}

	for (int i = 0; i < 2; ++i) {
		score[i]->writetext(int2str(data->tm[i].sc), 39, -9, 1);
		//name[i]->writetext(data->name[i], 57, -5, 1);
		name[i]->writetextshrink(data->tm[i].name, 57, 16, 1, 86);
		bool name_changed = dark_name[i]->writetextshrink(data->tm[i].name, 57, 17, 1, 86);
		score[i]->painton(main, 174+176*i, 14, 1.0);
		name[i]->painton(main, 74+176*i, 16, 1.0);

		if (name_changed) {
			ync[i]->clear();
			yellowname->painton(ync[i], 0, 0, 1.0);
			dark_name[i]->painton(ync[i], 0, -1, 1.0);
		}
	}

	if (data->active_clock == &(data->int_clock)) {
		if (data->use_tenths) clock->writetext(getclock(min,sec,tenths), 66, -9, 1);
		else clock->writetext(getclock_nt(min,sec,tenths), 66, -9, 1);
		period->clear();
	}
	else if (data->period == 127) {
		clock->writetextshrink(string("FINAL"), 66, 18, 1, 110);
		period->clear();
	}
	else {
		if (data->use_tenths) clock->writetext(getclock(min,sec,tenths), 66, -9, 1);
		else clock->writetext(getclock_nt(min,sec,tenths), 66, -9, 1);
		period->writetextshrink(data->getStringPeriod(), 31, 18, 1, 35);
	}


	clock->painton(main, 770, 14, 1.0);
	period->painton(main, 888, 14, 1.0);

	drop->display(main, data);
	
	float user_yellow = 0.0f;
	float y[2];
	drop->getyellow(y);
	if (data->yellow_v) user_yellow = 1.0f;
	if (data->yellow_v || y[0] > 0) ync[0]->painton(main, 74, 16, max(user_yellow, y[0]));
	if (data->yellow_h) user_yellow = 1.0f;
	if (data->yellow_h || y[1] > 0) ync[1]->painton(main, 250, 16, max(user_yellow, y[1]));

	main->paintgl(0.0f, 736.0f + (float) offset);
}

void HockeyDraw::update_team_color(int team, int r, int g, int b) {
	team_mask->fill_match_a(r, g, b, 255);
	team_mask->painton(bg, 74+176*team, 16, 1.0);
	team_gloss->painton(bg, 74+176*team, 16, 1.0);
}

bool drawRect(int xl, int xh, int yl, int yh, int z) {
	glBegin(GL_QUADS);
		glVertex3i(xl, yl, z);
		glVertex3i(xh, yl, z);
		glVertex3i(xh, yh, z);
		glVertex3i(xl, yh, z);
	glEnd();
	return true;
}
