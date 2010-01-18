#ifndef _hockeyroster_h_
#define _hockeyroster_h_

#include <vector>
#include <string>
#include <map>
#include "IniParser.h"
using namespace std;



class Roster {
private:
	string* roster[100];

public:
	Roster();
	~Roster();
	string* get(int number);
	void set(int number, const string& name);
};

typedef map<string, Roster*> mapsr;

class RosterList {

private:
	mapsr* teams;
	Roster* blank;

public:
	RosterList() {}
	~RosterList();
	RosterList(const string& filename);
	Roster* get_roster(const string& team);
	string get_string(Roster* r, int number);
	string get(const string& team, int number);
	unsigned int team_count();
	unsigned int count(const string& team);

};

#endif