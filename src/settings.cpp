#include "main.h"

void load_settings(const char* filename) {
	IniParser ip;
	ip.readfile(string(filename), ";#");
	string in, temp;
	int temp_int;

	// General section
	if (ip.get("General", "PeriodLength", in)) {
		temp_int = 1200;	// default of 20 minutes
		IniParser::parse(in, temp_int);
		if (temp_int <= 0) temp_int = 1200;
		temp_int *= 1000;
		data->PERLEN = temp_int;
		data->clock.set(temp_int);
	}
	if (ip.get("General", "IntermissionLength", in)) {
		temp_int = 720;		// default of 12 minutes
		IniParser::parse(in, temp_int);
		if (temp_int <= 0) temp_int = 720;
		temp_int *= 1000;
		data->INTLEN = temp_int;
		data->int_clock.set(temp_int);
	}
	if (ip.get("General", "OvertimeLength", in)) {
		temp_int = 300;		// default of 5 minutes
		IniParser::parse(in, temp_int);
		if (temp_int <= 0) temp_int = 300;
		data->otlen = 1000 * temp_int;
	}
	if (ip.get("General", "KeyMode", in)) {
		if (in[0] == 'l' || in[0] == 'L') data->keymode = 'l';
		else if (in[0] == 'g' || in[0] == 'G') data->keymode = 'g';
		else if (in[0] == 'b' || in[0] == 'B') data->keymode = 'b';
		else if (in[0] == 'x' || in[0] == 'X') data->keymode = 'x';
	}
	if (ip.get("General", "YOffset", in)) {
		temp_int = 0;
		IniParser::parse(in, temp_int);
		if (temp_int >= -64 && temp_int <= 32) hd->offset = temp_int;
	}
	if (ip.get("General", "RosterFile", in)) {
		data->roster_file = in;
		data->reloadRosters();
	}
	if (ip.get("General", "GraphicsFile", in)) {
		// isn't used yet
	}
	
	// Effects section
	if (ip.get("Effects", "InfoFadeTime", in)) {
		temp_int = 100;
		IniParser::parse(in, temp_int);
		if (temp_int >= 0) drop->TK = (float) temp_int / 1000.0f;
	}
	if (ip.get("Effects", "GoalFlashTime", in)) {
		temp_int = 4000;
		IniParser::parse(in, temp_int);
		if (temp_int > 0) data->FLASH_LEN = temp_int;
	}
	if (ip.get("Effects", "GoalFlashCycle", in)) {
		temp_int = 250;
		IniParser::parse(in, temp_int);
		if (temp_int > 0) data->FLASH_CYC = temp_int;
	}
	if (ip.get("Effects", "GoalFlashOff", in)) {
		temp_int = 125;
		IniParser::parse(in, temp_int);
		if (temp_int > 0) data->FLASH_OFF = temp_int;
	}

	// Sync section
	if (ip.get("Sync", "Port", in)) {
		temp_int = 0;
		IniParser::parse(in, temp_int);
		if (temp_int > 0) data->ssync.port_open(temp_int);
	}
	if (ip.get("Sync", "StartDelay", in)) {
		temp_int = 0x80000000;
		IniParser::parse(in, temp_int);
		if (temp_int != 0x80000000) data->start_delay = temp_int;
	}
	if (ip.get("Sync", "StopDelay", in)) {
		temp_int = 0x80000000;
		IniParser::parse(in, temp_int);
		if (temp_int != 0x80000000) data->stop_delay = temp_int;
	}

	string team[2];
	team[0] = "Visitor";
	team[1] = "Home";
	// Team section
	for (int i = 0; i < 2; ++i) {
		if (ip.get(team[i], "Name", in)) {
			data->tm[i].name = in;
			drop->setstring(SI_TO_V + i, in + " TIMEOUT");
		}
		if (ip.get(team[i], "LongName", in)) {
			data->tm[i].lname = in;
			drop->setstring(SI_TO_V + i, in + " TIMEOUT");
		}
		if (ip.get(team[i], "FullName", in)) data->tm[i].fname = in;
		if (ip.get(team[i], "RosterName", in)) data->tm[i].rs = in;
		if (ip.get(team[i], "BackColor", in)) {
			vector<string> vs;
			IniParser::trim(in);
			IniParser::remove_dup_delim(in, ",");
			IniParser::parsedelim(in, ",", vs);
			if (vs.size() == 3) {
				int r = -1, g = -1, b = -1;
				IniParser::parse(vs[0], r);
				IniParser::parse(vs[1], g);
				IniParser::parse(vs[2], b);
				if (r >= 0 && r <= 255 && g >= 0 && g <= 255 && b >= 0 && b <= 255) {
					hd->update_team_color(i, r, g, b);
				}
			}
		}
		if (ip.get(team[i], "ForeColor", in)) {
			vector<string> vs;
			IniParser::trim(in);
			IniParser::remove_dup_delim(in, ",");
			IniParser::parsedelim(in, ",", vs);
			if (vs.size() == 3) {
				int r = -1, g = -1, b = -1;
				IniParser::parse(vs[0], r);
				IniParser::parse(vs[1], g);
				IniParser::parse(vs[2], b);
				if (r >= 0 && r <= 255 && g >= 0 && g <= 255 && b >= 0 && b <= 255) {
					hd->name[i]->color(r,g,b);
				}
			}
		}
	}

	// Text section
	for (int i = MIN_USER_STATE; i <= MAX_USER_STATE; ++i) {
		if (ip.get("Text", IniParser::to_str(i), in)) {
			drop->setstring(i, in);
		}
	}
}