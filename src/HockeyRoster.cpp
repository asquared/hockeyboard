#include "HockeyRoster.h"

void make_uppercase(string& s) {
	for (unsigned int i = 0; i < s.length(); ++i) {
		if (s[i] >= 'a' && s[i] <= 'z') s[i] -= 32;
	}
}

Roster::Roster() {
	for (int i = 0; i < 100; ++i) roster[i] = 0;
}
Roster::~Roster() {
	for (int i = 0; i < 100; ++i) {
		if (roster[i]) delete roster[i];
	}
}
string* Roster::get(int number) {
	if (number >= 0 && number < 100) 
		return roster[number];	
	return 0;
}
void Roster::set(int number, string& name) {
	if (number < 0 || number >= 100) return;
	if (roster[number]) delete roster[number];
	roster[number] = new string(name);
}


RosterList::RosterList(string& filename) {
	teams = new mapsr;
	
	IniParser ip;
	if (!(ip.readfile(filename, ";#"))) return;

	blank = new Roster;
	//cout << "blank: " << blank << endl;
	teams->insert(make_pair("", blank));  // dummy blank roster, used for non-existant team name

	vector<string> group_list;
	ip.get_groups(group_list);
	vector<string>::iterator itr = group_list.begin();
	for ( ; itr != group_list.end(); ++itr) {
		int count = 0;
		string team_name = *itr;
		Roster* a = new Roster;
		teams->insert(make_pair(team_name, a));
		string strnum;
		string name = "";
		for (int i = 0; i < 100; ++i) {
			strnum = IniParser::int2str(i);
			name = "";
			ip.get(team_name, strnum, name);
			if (name != "") {
				++count;
				make_uppercase(name);
			}
			a->set(i, name);
		}
		//cout << team_name << ": " << a << endl;
		cout << team_name << ' ' << count << '\n';
	}
	//cout << '\"' << get_string(blank, 99) << '\"' << endl;
}

RosterList::~RosterList() {
	for (mapsr::iterator i = teams->begin(); i != teams->end(); ++i) {
		delete i->second;
	}
	delete teams;
}

Roster* RosterList::get_roster(string& team) {
	mapsr::iterator i = teams->find(team);
	if (i != teams->end()) return i->second;
	return blank;
}

string RosterList::get_string(Roster* r, int number) {
	if (r) {
		string* s = r->get(number);
		if (s) return *s;
	}
	return string("");
}

string RosterList::get(string& team, int number) {
	mapsr::iterator i = teams->find(team);
	if (number < 0 || number >= 100 || i == teams->end()) 
		return string("");
	return get_string(i->second, number);
}

unsigned int RosterList::team_count() {
	return max((unsigned int) 1, (unsigned int) teams->size()) - 1;
}

unsigned int RosterList::count(string& team) {
	Roster* r = get_roster(team);
	if (r == 0 || r == blank) return 0;
	int out = 0;
	for (int i = 0; i < 100; ++i) {
		if ( r->get(i) != 0 && *(r->get(i)) != "" ) ++out;
	}
	return out;
}

