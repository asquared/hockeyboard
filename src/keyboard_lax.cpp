#ifdef LAX
#include "main.h"

// jump to state using Alt-number
const int jumpstate[] = {46, 0, 6, 12, 20, 30, 33, 36, 39, 42};

const int penaltykeys[] = {'p',';','[','\'','p',':','?','{','\"','}'};

typedef void (*keyfunc)(int, int);

void clock_adjust(int mod, int time) {
	data->active_clock->adjust(time);
}

void drop_change_state(int mod, int num) {
	drop->set_state(drop->user_state + num);
}

void drop_toggle(int mod, int state) {
	drop->toggle(state);
}
void drop_toggle_pp(int mod, int x) {
	//if (mod & GLUT_ACTIVE_SHIFT) drop->toggle(SI_PP_EN);
	//else drop->toggle(SI_PP);
	drop->toggle(SI_PP);
}
void drop_toggle_enps(int mod, int team) {
	//if (mod & GLUT_ACTIVE_SHIFT) drop->toggle(SI_PS_V + team);
	//else drop->toggle(SI_EN_V + team);
}
void drop_user(int mod, int x) {
	drop->drop(drop->user_state);
}
void drop_raise(int mod, int x) {
	drop->raise();
}

void score(int mod, int team) {
	if (!(mod & GLUT_ACTIVE_SHIFT) && data->tm[team].sc < 255) ++data->tm[team].sc;
	else if ((mod & GLUT_ACTIVE_SHIFT) && data->tm[team].sc > 0) --data->tm[team].sc;
}
void score_red(int mod, int team) {
	if (!(mod & GLUT_ACTIVE_SHIFT)) data->startRedFlash(team);
	else data->stopRedFlash(team);
}

void keyboard( unsigned char key, int x, int y ) {
	//cout << int2str(key) << endl;

	// Del (or Ctrl-Bksp)
	if ( key == 127 ) {
		data->active_clock->stop();
		return;
	}

	int mod = glutGetModifiers();
	//cout << "mod: " << int2str(mod) << endl;

	// Esc
	if (key == 27) {
		if (mod == GLUT_ACTIVE_SHIFT && logic->isClear()) {
			logic->next = 31;
			logic->selectMajor(data, hd, kbuf, drop);
		}
		else logic->clear();
		return;
	}
	// Alt-[0-9]
	if (mod == GLUT_ACTIVE_ALT) {
		if (key >= '0' && key <= '9') drop->user_state = jumpstate[key-'0'];
		else if (key == '-') drop->user_state = 50;
		else if (key == '=') drop->user_state = SI_SOG;
	}
	// Spacebar and numbers (outside of prompts)
	if (logic->isClear()) {
		if (key == 32) data->active_clock->toggle();
		else if (key == 'y') {
			data->sync_ignore_count = 2;
			data->sync = !(data->sync);
		}
		else if (key >= '0' && key <= '9') drop->user_state = jumpstate[key-'0'];
		else if (key == '-') drop->user_state = 50;
		else if (key == '=') drop->user_state = SI_SOG;
		else if (key == '<') data->yellow_v = !(data->yellow_v);
		else if (key == '>') data->yellow_h = !(data->yellow_h);
		else if (key == 'q') {
			if (data->tm[0].sog < 255) ++(data->tm[0].sog);
		}
		else if (key == 'Q') {
			if (data->tm[0].sog > 0) --(data->tm[0].sog);
		}
		else if (key == 'a') {
			if (data->tm[1].sog < 255) ++(data->tm[1].sog);
		}
		else if (key == 'A') {
			if (data->tm[1].sog > 0) --(data->tm[1].sog);
		}

		// Tab
		else if (mod != GLUT_ACTIVE_CTRL && key == 9) {
			logic->next = 30;
			logic->selectMajor(data, hd, kbuf, drop);
		}
		// Ctrl-[A-Z]
		else if (mod == GLUT_ACTIVE_CTRL) {
			logic->next = key;
			logic->selectMajor(data, hd, kbuf, drop );
		}
		// new exit code
		else if (mod == GLUT_ACTIVE_SHIFT && key == 27) {
			quit(0);
		}

		if (key == 'P') key = 'p';
		if ( !(mod & GLUT_ACTIVE_SHIFT) ) {
			for (int i = 0; i < 4; ++i) {
				if (key == penaltykeys[i]) {
					if (mod & GLUT_ACTIVE_ALT) 
						data->addPenalty(i >> 1, data->period, data->clock.read(), 4 + 2*(i&1));
					else
						data->addPenalty(i >> 1, data->period, data->clock.read(), 1 + (i&1));
				}
			}
		}
		else {
			for (int i = 0; i < 6; ++i) {
				if (key == penaltykeys[i+4]) {
					data->delPenalty(i / 3, i % 3);
				}
			}
		}
	}
	// others
	else {
		kbuf->add(key);
		logic->selectMajor(data, hd, kbuf, drop );
	}	
}

// F1-F12
const keyfunc fkeys[12] = {
	drop_toggle, drop_toggle_pp, drop_toggle_enps, drop_toggle_enps,
	drop_toggle, drop_toggle, drop_user, drop_raise,
	score, score, score_red, score_red
};

const int fkeys_p[12] = {
	SI_DP, 0, 0, 1,
	SI_TO_V, SI_TO_H, 0, 0,
	0, 1, 0, 1
};

// LEFT, UP, RIGHT, DOWN, PGUP, PGDN, HOME, END
const keyfunc extkeys[8] = {
	NULL, drop_change_state, NULL, drop_change_state,
	clock_adjust, clock_adjust, clock_adjust, clock_adjust
};

const int extkeys_p[8] = {
	0, -1, 0, 1,
	100, -100, 1000, -1000
};

void specialkey( int key, int x, int y ) {
	//cout << "sp:" << int2str(key) << endl;
	
	int mod = glutGetModifiers();
	
	// clock controls (except DELETE)
	if (key == GLUT_KEY_INSERT) data->active_clock->start();
	else if (key >= GLUT_KEY_LEFT && key <= GLUT_KEY_END) {
		if (extkeys[key-GLUT_KEY_LEFT]) (*extkeys[key-GLUT_KEY_LEFT])(mod, extkeys_p[key-GLUT_KEY_LEFT]);
	}
	// drop controls
	// score and yellow highlight controls
	else if (key >= GLUT_KEY_F1 && key <= GLUT_KEY_F12) {
		if (fkeys[key-GLUT_KEY_F1]) (*fkeys[key-GLUT_KEY_F1])(mod, fkeys_p[key-GLUT_KEY_F1]);
	}
}
#endif
