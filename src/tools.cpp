//
// tools.cpp
// utility functions for mc
//
// Created by Daniel Kozitza
//

#include "tools.hpp"
#include <iostream>
#include <fstream>

using namespace std;

void tools::get_vfnmake_conf(unordered_map<string, string>& config) {

	ifstream fh;
	fh.open("vfnmake.conf", ifstream::in);
	if (!fh.is_open()) {
		cout << "mc: could not open vfnmake.conf\n";
		return;
	}
	while(fh.peek() != EOF) {
		string line;
		getline(fh, line);

		string key = line;
		string val = line;

		if (line[0] == '#')
			continue;

		// get the key
		int colon_i = key.find(":");
		if (colon_i == 0 || colon_i == string::npos)
			continue;
		key.resize(colon_i);

		// get the value
		val.replace(0, colon_i+1, "");
		while (val.find(" ") == 0) {
			val.replace(0, 1, "");
		}

		config[key] = val;
	}

	fh.close();
}
