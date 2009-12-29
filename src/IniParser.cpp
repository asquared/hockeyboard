#include "IniParser.h"

IniParser::IniParser() {

}

IniParser::~IniParser() {

}

// useful function: check if c is a char in delim
inline bool isdelim( char c, const string& delim ) {
	for ( unsigned int i = 0 ; i < delim.size(); ++i ) {
		if ( c == delim[i] ) return true;
	}
	return false;
}

// splitting function
inline bool split(char* in, string& line, string& value) {
	line.clear();
	value.clear();

	while ( *in != '=' && *in != 0 ) {
		line.push_back(*in);
		++in;
	}
	if ( *in == 0 ) return false;
	++in;
	while ( *in != 0 ) {
		value.push_back(*in);
		++in;
	}
	return true;
}

// group function
inline bool readgroup(char* in, string& group) {
	group.clear();
	while ( *in != ']' && *in != 0 ) {
		group.push_back(*in);
		++in;
	}
	if ( *in == 0 ) return false;
	else return true;
}

bool IniParser::readfile(const string& file, const string& commentchars) {
	char c[256];
	char* ci;
	string currgroup("");
	string group, line, value;
	m.clear();

	fstream f(file.c_str(), fstream::in);
	if ( f.fail() ) return false;

	while ( !f.eof() ) {
		f.getline(c, 256);
		ci = c;
		while ( *ci > 0 && *ci <= 32 ) ++ci;
		if ( *ci != 0 && !isdelim( *ci, commentchars ) ) {
			if ( *ci == '[' ) {
				if ( readgroup( ci+1, group ) ) currgroup = group;
			}
			else {
				if ( split( ci, line, value ) ) {
					// insert into map
					m[currgroup][line] = value;
				}
			}
		}
	}

	return true;
}

bool IniParser::get(const string& group, const string& line, string& value) {
	bigmap::iterator bm = m.find(group);
	if ( bm == m.end() ) return false;
	strmap::iterator sm = bm->second.find(line);
	if ( sm == bm->second.end() ) return false;
	value = sm->second;
	return true;

}

bool IniParser::get_groups(vector<string>& group_list) {
	bigmap::iterator bm = m.begin();
	if (bm == m.end()) return false;
	for ( ; bm != m.end(); ++bm) {
		group_list.push_back(bm->first);
	}
	return true;
}

int IniParser::parseint(const string& str, int defval) {
	stringstream ss;
	ss << str;
	int out = defval;
	ss >> out;
	return out;

}

string IniParser::int2str(int in) {
	stringstream ss;
	ss << in;
	return ss.str();
}

bool IniParser::parsedelim(const string& in, const string& delim, vector<string>& out) {
	string temp;
	out.clear();
	if ( in == "" ) return true;
	for ( unsigned int i = 0; i < in.size(); ++i ) {
		if ( isdelim( in[i], delim ) ) { 
			out.push_back(temp);
			temp.clear();
		}
		else {
			temp.push_back(in[i]);
		}
	}
	out.push_back(temp);
	return true;
}

void IniParser::trim(string& in) {
	unsigned int size = (unsigned int) in.size();
	unsigned int start_pos = 0, end_pos = size - 1;
	for (unsigned int i = 0; i < size; ++i) {
		if (in[i] != ' ') {
			start_pos = i;
			break;
		}
	}
	for (unsigned int i = size - 1; i >= 0; --i) {
		if (in[i] != ' ') {
			end_pos = i;
			break;
		}
	}
	if (end_pos - start_pos + 1 <= 0) in = "";
	else in = in.substr(start_pos, end_pos - start_pos + 1);
}

void IniParser::remove_dup_delim(string& in, const string& delim) {
	bool clear_now = false;
	for (unsigned int i = 0; i < in.size(); ++i) {
		if ( isdelim(in[i], delim) ) {
			if (!clear_now) clear_now = true;
			else {
				in.erase(i, 1);
				--i;
			}
		}
		else clear_now = false;
	}
}
