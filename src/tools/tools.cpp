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

// add_documentation
//
// adds title to source files and descriptions to functions. if the first line
// of the file is not "//" it prepends a title to the file after prompting for
// a description of the file. if the line before a function declaration is not
// "//" it prepends a description of the function after prompting the user.
//
void tools::add_documentation(string fname) {
	ifstream ifh;
	ifh.open(fname, ifstream::in);
	if (!ifh.is_open()) {
		cout << "tools::add_documentation: couldn't open " << fname << ".\n";
		return;
	}

	ofstream ofh;
	ofh.open(fname + "_mc_added_docs", ofstream::out);
	if (!ofh.is_open()) {
		cout << "tools::add_documentation: couldn't open " << fname;
		cout << "_mc_added_docs.\n";
	}

	// the title starts on line 1 and ends before the first uncommented line.
	vector<string> title;

	// prefunc is the comment block that will be written before a function.
	vector<string> prefunc;
	string last_line = "NO_LAST_LINE";

	int line_num = 1;
	while (ifh.peek() != EOF) {
		string line;
		getline(ifh, line);

		if (line_num == 1)  {

			if (line != "//") {
				cout << "tools::add_documentation: making new title.\n";
				cout << "   lines will be broken at 80 automatically, \\n will be ";
				cout << "replaced with newlines.\n";
				cout << "   enter a description for the title:\n";
				string desc;
				getline(cin, desc);
				
				cout << "   adding title:\n\n";
				title.push_back("//");
				title.push_back("// " + fname); // TODO: get rid of dir names here
				title.push_back("//");
				if (desc != "") {
					title.push_back("// " + desc); // TODO: break lines at 80
					title.push_back("//");
				}
				title.push_back("// Created by Daniel Kozitza");
				title.push_back("//");
				for (const auto item : title)
					cout << "   " << item << "\n";
				cout << "\n";
				for (const auto item : title)
					ofh << item << endl;

			}
			else {
				cout << "tools::add_documentation: file already has a title\n";
			}
		}

		line_num++;
	}

	ifh.close();
	ofh.close();
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

