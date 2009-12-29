#include "HockeyDraw.h"

inline void set_font_defaults(GLCairoSurface* glcs, int size) {
	glcs->setfontface("Franklin Gothic Demi Cond", false, false, false);
	//glcs->setfontface("Gotham Narrow Bold", false, false, false);
	glcs->setfontsize(size);
	glcs->color(240,240,240);
}

inline void set_font_alternate(GLCairoSurface* glcs, int size) {
	glcs->setfontface("Franklin Gothic Demi Cond", false, false, false);
	glcs->setfontfacealt(0, "Franklin Gothic Demi Comp", false, false, false);
	//glcs->setfontface("Gotham Narrow Bold", false, false, false);
	//glcs->setfontfacealt(0, "Gotham XNarrow Bold", false, false, false);
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


	score[0] = new GLCairoTextSurface(82, 40);
	score[1] = new GLCairoTextSurface(82, 40);
	name[0] = new GLCairoTextSurface(111, 34);
	name[1] = new GLCairoTextSurface(111, 34);
	dark_name[0] = new GLCairoTextSurface(111, 34);
	dark_name[1] = new GLCairoTextSurface(111, 34);
	clock = new GLCairoTextSurface(150, 40);
	period = new GLCairoTextSurface(75, 40);
	main = new GLCairoSurface(1024, 64);
	ync[0] = new GLCairoSurface(117, 38);
	ync[1] = new GLCairoSurface(117, 38);

	try {
		bg = new GLCairoSurface("hb_background.png");
		team = new GLCairoSurface("hb_team.png");
		clockper = new GLCairoSurface("hb_clock.png");
		redscore = new GLCairoSurface("hb_redscore.png");
		yellowname = new GLCairoSurface("hb_yellowteam.png");
		team_mask = new GLCairoSurface("team_mask.png");
		team_gloss = new GLCairoSurface("team_gloss.png");
	}
	catch (int) {
		std::cout << "Error: PNG file not found or read error\n";
	}

	team->painton(bg, 71, 10, 1.0);
	team->painton(bg, 248, 10, 1.0);
	clockper->painton(bg, 751, 10, 1.0);

	set_font_defaults(clock, 36);
	//clock->setfontface("Gotham Narrow Bold FWN", false, false, false);
	set_font_defaults(period, 36);
	set_font_defaults(score[0], 36);
	set_font_defaults(score[1], 36);
	set_font_alternate(name[0], 30);
	set_font_alternate(name[1], 30);
	set_font_alternate(dark_name[0], 30);
	set_font_alternate(dark_name[1], 30);
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
	get_clock_parts(*(data->active_clock), min, sec, tenths);

	main->clear();
	bg->painton(main, 0, 0, 1.0);

	// red flash
	if (data->getRedFlash()) {
		redscore->painton(main, 167+177*(data->red_flash_team), 10, 1.0);
	}

	for (int i = 0; i < 2; ++i) {
		score[i]->writetext(int2str(data->sc[i]), 41, -10, 1);
		//name[i]->writetext(data->name[i], 55, -8, 1);
		name[i]->writetextshrink(data->name[i], 55, 17, 1, 82);
		bool name_changed = dark_name[i]->writetextshrink(data->name[i], 55, 17, 1, 82);
		score[i]->painton(main, 170+177*i, 12, 1.0);
		name[i]->painton(main, 74+177*i, 15, 1.0);

		if (name_changed) {
			ync[i]->clear();
			yellowname->painton(ync[i], 0, 0, 1.0);
			dark_name[i]->painton(ync[i], 3, 2, 1.0);
		}
	}

	if (data->active_clock == &(data->int_clock)) {
		clock->writetext(getclock(min,sec,tenths), 75, -10, 1);
		period->clear();
	}
	else if (data->period == 127) {
		clock->writetext(string("FINAL"), 75, -10, 1);
		period->clear();
	}
	else {
		clock->writetext(getclock(min,sec,tenths), 75, -10, 1);
		period->writetextshrink(data->getStringPeriod(), 37, 20, 1, 45);
	}


	clock->painton(main, 751, 12, 1.0);
	period->painton(main, 879, 12, 1.0);

	float alpha = drop->display(main);
	
	float user_yellow = 0.0f;
	if (data->yellow_v) user_yellow = 1.0f;
	if (data->yellow_v || (drop->getyellow() == 1)) ync[0]->painton(main, 71, 13, max(user_yellow, alpha));
	if (data->yellow_h) user_yellow = 1.0f;
	if (data->yellow_h || (drop->getyellow() == 2)) ync[1]->painton(main, 248, 13, max(user_yellow, alpha));

	main->paintgl(0.0f, 736.0f + (float) offset);
}

void HockeyDraw::update_team_color(int team, int r, int g, int b) {
	team_mask->fill_match_a(r, g, b, 255);
	team_mask->painton(bg, 74+177*team, 15, 1.0);
	team_gloss->painton(bg, 74+177*team, 15, 1.0);
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