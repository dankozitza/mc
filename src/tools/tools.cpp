//
// tools.cpp
// utility functions for mc
//
// Created by Daniel Kozitza
//

#include "../tools.hpp"
#include <iostream>
#include <fstream>

// get_vfnmake_conf
//
// Reads vnfmake.conf variables into 'config'.
//
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

	string tfname = fname + "_mc_added_docs";
	string answer;
	bool made_change = false;

	ifstream ifh;
	ifh.open(fname, ifstream::in);
	if (!ifh.is_open()) {
		cout << "tools::add_documentation: couldn't open `" << fname << ".\n";
		return;
	}

	ofstream ofh;
	ofh.open(tfname, ofstream::out);
	if (!ofh.is_open()) {
		cout << "tools::add_documentation: couldn't open `" << tfname << ".\n";
		return;
	}


	string previous_line = "NO_PREVIOUS_LINE";

	int line_num = 1;
	while (ifh.peek() != EOF) {
		string line;
		getline(ifh, line);

		if (line_num == 1)  {
			vector<string> title;

			if (line != "//") {
				cout << "tools::add_documentation: making new title.\n";
				cout << "   lines will be broken at 80 automatically, \\n will be ";
				cout << "replaced with newlines.\n";
				cout << "   enter a description for the title:\n\n";
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
				made_change = true;

			}
			else {
				cout << "tools::add_documentation: file already has a title.\n";
			}
		}

		vector<string> m;
		if (matches(m, line, R"(\w.*?(\w+)\(.*\) \{)")) {
			// prefunc is the comment block that will be written before a function.
			vector<string> prefunc;

			if (previous_line != "//") {
				cout << "tools::add_documentation: found function declaration ";
				cout << "without documentation:\n";
				cout << "   " << fname << " line " << line_num << ":\n\n";
				cout << line << "\n\n";
				cout << "   add description? (Y/n): ";
				getline(cin, answer);
				if (answer != "n") {
					cout << "   lines will break at 80 automatically, \\n will be ";
					cout << "replaced with newlines.\n";
					cout << "   enter a description for the function `" << m[1];
					cout << "`:\n\n";
					string desc;
					getline(cin, desc);
					
					cout << "   adding description:\n\n";
					prefunc.push_back("// " + m[1]); // TODO: get rid of dir names here
					prefunc.push_back("//");
					if (desc != "") {
						prefunc.push_back("// " + desc); // TODO: break lines at 80
						prefunc.push_back("//");
					}
					for (const auto item : prefunc)
						cout << item << "\n";
					cout << "\n";
					for (const auto item : prefunc)
						ofh << item << endl;
					made_change = true;
				}
				else
					cout << endl;
			}
		}

		// copy the lines to the new file
		ofh << line << endl;

		previous_line = line;
		line_num++;
	}

	ifh.close();
	ofh.close();

	if (!made_change) {
		cout << "tools::add_documentation: no changes made!\n";
		cout << "   removing `" << tfname << "`.\n";
		remove(tfname.c_str());
		return;
	}

	cout << "tools::add_documentation: finished!\n   new file is `" << tfname;
	cout << "`.\n\n";
	cout << "   overwrite existing file? (Y/n): ";

	getline(cin, answer);

	if (answer != "n") {
		cout << "   removing `" << fname << "`.\n";
		remove(fname.c_str());
		cout << "   renaming `" << tfname << "` to `" << fname << "`.\n";
		rename(tfname.c_str(), fname.c_str());
	}
	else {
		cout << "   overwrite canceled!\n";
		cout << "\n   remove `" << tfname << "`? (Y/n): ";
		getline(cin, answer);
		if (answer != "n") {
			cout << "   removing `" << tfname << "`.\n";
			remove(tfname.c_str());
		}
	}
}

// require
//
// overloaded function for exiting on failure with optional message.
//
// TODO: have this throw an exception instead.
//
// exit if sys_exit_val is not zero.
//
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

// require
//
// exit if func_return_val is false
//
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

