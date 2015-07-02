//
// tools.cpp
// utility functions for mc
//
// Created by Daniel Kozitza
//

#include "../tools.hpp"
#include <iostream>
#include <fstream>

bool tools::get_vfnmake_conf(unordered_map<string, string>& config) {

	ifstream fh;
	fh.open("vfnmake.conf", ifstream::in);
	if (!fh.is_open()) {
		cout << "tools::get_vfnmake_conf: couldn't open vfnmake.conf.\n";
		return false;
	}
	while(fh.peek() != EOF) {
		string line;
		getline(fh, line);

		string m[3];
		if (matches(m, line, "^(\\w+):\\s*(.*)$"))
			config[m[1]] = m[2];
	}

	fh.close();
	return true;
}

// require
//
// overloaded function for exiting on failure with optional message.
//
// TODO: have this throw an exception instead.
//
// exit if sys_exit_val is not zero.
bool tools::require(int sys_exit_val, string msg) {
	if (sys_exit_val != 0) {
		if (msg != "") {
			cout << msg << "\n";
		}
		cout << "tools::require: got exit value `" << sys_exit_val << "`, ";
		cout << "exiting.\n";
		exit(EXIT_FAILURE);
	}
}
// exit if func_return_val is false
bool tools::require(bool func_return_val, string msg) {
	if (func_return_val == false) {
		if (msg != "") {
			cout << msg << "\n";
		}
		cout << "tools::require: got return value `false`, ";
	   cout << "exiting.\n";
		exit(EXIT_FAILURE);
	}
}

