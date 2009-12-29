#ifndef _iniparser_h_
#define _iniparser_h_

#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
using namespace std;

typedef map< string, map< string, string > > bigmap;
typedef map< string, string > strmap;

class IniParser {
private:
	bigmap m;

public:
	IniParser();
	~IniParser();

	bool readfile(const string& file, const string& commentchars);
	bool get(const string& group, const string& line, string& value);

	bool get_groups(vector<string>& group_list);

	static int parseint(const string& str, int defval);
	static string int2str(int in);
	
	template <class T>
	static string to_str(T& in);
	
	template <class T>
	static void parse(const string& str, T& out);

	static bool parsedelim(const string& in, const string& delim, vector<string>& out);

	static void trim(string& in);
	static void remove_dup_delim(string& in, const string& delim);

};

template <class T>
string IniParser::to_str(T& in) {
	stringstream ss;
	ss << in;
	return ss.str();
}

template <class T>
void IniParser::parse(const string& str, T& out) {
	stringstream ss;
	ss << str;
	ss >> out;
}

#endif