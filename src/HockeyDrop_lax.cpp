#ifdef LAX
#include "HockeyDrop.h"

// ppstate:  -1: full strength  0: even strength  1: vis power play  2: home power play
void HockeyDrop::ppdata(short adv, unsigned short strength, unsigned short pmin, unsigned short psec) {
	if ( adv == 0 ) {
		if (strength == 10) {
			lines[SI_PP] = "FULL STRENGTH";
			lines[SI_PP_EN] = "EMPTY NET";
			settime(0,0);
			ppstate = -1;
		}
		else {
			if (strength == 9) {
				lines[SI_PP] = "9-ON-9";
				lines[SI_PP_EN] = "9-ON-9";
			}
			else if (strength == 8) {
				lines[SI_PP] = "8-ON-8"; 
				lines[SI_PP_EN] = "8-ON-8";
			}
			else if (strength == 7) {
				lines[SI_PP] = "7-ON-7"; 
				lines[SI_PP_EN] = "7-ON-7";
			}
			settime(pmin, psec);
			ppstate = 0;
		}
	}
	else if ( abs(adv) == 1 ) {
		if (strength == 8) {
			lines[SI_PP] = "9-ON-8";
			lines[SI_PP_EN] = "9-ON-8";
		}
		else if (strength == 7) {
			lines[SI_PP] = "8-ON-7";
			lines[SI_PP_EN] = "8-ON-7";
		}
		else {
			lines[SI_PP] = "MAN UP";
			lines[SI_PP_EN] = "MAN UP";
		}
	}
	else if ( abs(adv) == 2 ) {
		if (strength == 7) {
			lines[SI_PP] = "9-ON-7";
			lines[SI_PP_EN] = "9-ON-7";
		}
		else {
			lines[SI_PP] = "2 MEN UP";
			lines[SI_PP_EN] = "2 MEN UP";
		}
	}
	else if ( abs(adv) == 3 ) {
		lines[SI_PP] = "3 MEN UP";
		lines[SI_PP_EN] = "3 MEN UP";
	}
	else state = -1;   // invalid case

	if ( abs(adv) >= 1 ) {
		settime(pmin, psec);
		if (adv > 0) ppstate = 1;
		else ppstate = 2;
	}
}

#endif