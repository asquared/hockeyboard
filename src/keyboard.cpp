#ifndef LAX
#include "main.h"

// jump to state using Alt-number
const int jumpstate[] = {46, 0, 6, 12, 20, 30, 33, 36, 39, 42};

const int penaltykeys[] = {'p',';','[','\'','P',':','{','\"'};

void keyboard( unsigned char key, int x, int y ) {
	//cout << "key: " << int2str(key) << "  ";

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
	// old raise key (now F8)
	//if ( key == '`' ) {
	//	drop->raise();
	//}
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

		for (int i = 0; i < 8; ++i) {
			if (key == penaltykeys[i]) {
				if (mod == 0) 
					data->addPenalty(i >> 1, data->period, data->clock.read(), 2 + 3*(i&1));
				if (mod == GLUT_ACTIVE_ALT) 
					data->addPenalty(i >> 1, data->period, data->clock.read(), 4 + 3*(i&1));
				if (mod == GLUT_ACTIVE_SHIFT) 
					data->delPenalty((i-4) >> 1, (i-4) & 1);
			}
		}
	}
	// others
	else {
		kbuf->add(key);
		logic->selectMajor(data, hd, kbuf, drop );
	}	
}

void specialkey( int key, int x, int y ) {
	//cout << "spk: " << int2str(key) << "  ";
	
	int mod = glutGetModifiers();
	//cout << "mod: " << int2str(mod) << endl;
	//if (mod == GLUT_ACTIVE_ALT && key == GLUT_KEY_F4) quit(0);
	
	// clock controls (except DELETE)
	if (key == GLUT_KEY_INSERT) data->active_clock->start();
	else if (key == GLUT_KEY_HOME) data->active_clock->adjust(1000);
	else if (key == GLUT_KEY_END) data->active_clock->adjust(-1000);
	else if (key == GLUT_KEY_PAGE_UP) data->active_clock->adjust(100);
	else if (key == GLUT_KEY_PAGE_DOWN) data->active_clock->adjust(-100);

	else if (key == GLUT_KEY_DOWN) {
		drop->set_state(drop->user_state + 1);
	}
	else if (key == GLUT_KEY_UP) {
		drop->set_state(drop->user_state - 1);
	}	
	
	// drop controls
	else if (key == GLUT_KEY_F1) {
		drop->toggle(SI_DP);
	}
	else if (key == GLUT_KEY_F2) {
		if (mod == GLUT_ACTIVE_SHIFT) drop->toggle(SI_PP_EN);
		else drop->toggle(SI_PP);
	}
	else if (key == GLUT_KEY_F3) {
		if (mod == GLUT_ACTIVE_SHIFT) drop->toggle(SI_PS_V);
		else drop->toggle(SI_EN_V);
	}
	else if (key == GLUT_KEY_F4) {
		if (mod == GLUT_ACTIVE_SHIFT) drop->toggle(SI_PS_H);
		else drop->toggle(SI_EN_H);
	}
	else if (key == GLUT_KEY_F5) drop->toggle(SI_TO_V);
	else if (key == GLUT_KEY_F6) drop->toggle(SI_TO_H);
	else if (key == GLUT_KEY_F7) {
		drop->drop(drop->user_state);
	}
	else if (key == GLUT_KEY_F8) drop->raise();

	// score and yellow highlight controls
	else if (key == GLUT_KEY_F9) {
		if (mod == 0 && data->tm[0].sc < 255) ++(data->tm[0].sc);
		else if (mod == GLUT_ACTIVE_SHIFT && data->tm[0].sc > 0) --(data->tm[0].sc);
	}
	else if (key == GLUT_KEY_F10) { 
		if (mod == 0 && data->tm[1].sc < 255) ++(data->tm[1].sc);
		else if (mod == GLUT_ACTIVE_SHIFT && data->tm[1].sc > 0) --(data->tm[1].sc);
	}
	else if (key == GLUT_KEY_F11) {
		if (mod == 0) data->startRedFlash(0);
		else if (mod == GLUT_ACTIVE_SHIFT) data->stopRedFlash(0);
	}
	else if (key == GLUT_KEY_F12) {
		if (mod == 0) data->startRedFlash(1);
		else if (mod == GLUT_ACTIVE_SHIFT) data->stopRedFlash(1);
	}

}
#endif
