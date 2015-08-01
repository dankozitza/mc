//
// mc.cpp
//
// manage c++ code
//
// Created by Daniel Kozitza
//

#include <iostream>
#include <cstdlib>
#include <string.h>
#include <csignal>
#include "tools.hpp"

using namespace tools;

string         Args;
vector<string> Argv;
string         VfnmakeSystemCall = "vfnmake";

int help_message() {
	cout << "\nmc is a tool for managing c++ source code.\n\n";
	cout << "Usage:\n\n   mc command [arguments]\n\n";
	cout << "Commands:\n\n";
	cout << "   help     - Show this help message\n";
	cout << "   makefile - Creates a make file by calling vfnmake <arguments>\n";
	cout << "   build    - Calls vfnmake <arguments> then make\n";
	cout << "   rebuild  - Calls make clean, vfnmake <arguments>, then make\n";
	cout << "   run      - Calls vfnmake, make, then ./program <arguments>\n";
	cout << "   doc      - Parses .cpp files adding documentation and\n";
	cout << "              prompting the user for function descriptions\n";
	// TODO:
	//cout << "   mkreadme - Make a README.md file from ./program <arguments>.\n";
	//cout << "              arguments is `help` by default.\n";
	cout << "   mkhead   - Ensures that all the functions listed in the given\n";
	cout << "              source files are found in the header file.\n";
	cout << "   env      - Displays the variables read from vfnmake.conf\n";
	cout << "\n";
	return EXIT_FAILURE;
}

void makefile() {
	cout << "mc: calling `" << VfnmakeSystemCall << "`.\n";
	require(system(VfnmakeSystemCall.c_str()));
}

void build() {
	makefile();
	require(system("make"));
}

void rebuild() {
	require(system("make clean"));
	build();
}

void run() {
	build();

	unordered_map<string, string> vfnconf;
	require(get_vfnmake_conf(vfnconf));

	string named_prog_call = "./";
	named_prog_call += vfnconf["name"];
	named_prog_call += Args;
	cout << "mc: calling `" << named_prog_call << "`.\n";
	require(system(named_prog_call.c_str()));
}

void doc() {
	vector<string> fnames = Argv;

	if (fnames.size() == 0) {
		// get .cpp file names by recursing through vfnconf["src_directory"]
		cout << "mc: automatic source file detection is under construction.\n";
	}
	for (int fn_i = 0; fn_i < fnames.size(); fn_i++) {
		cout << "mc: adding documentation to `" << fnames[fn_i] << "`.\n";
		if (fnames.size() > 1)
			cout << "mc: file " << fn_i+1 << " of " << fnames.size() << ".\n";
		add_documentation(fnames[fn_i]);
		//for (const auto file_name : Argv)
		//	cout << file_name << "\n";// " << item.second << endl;
	}
}

void mkhead() {
	vector<string> fnames = Argv;

	if (fnames.size() == 0) {
		cout << "mc: automatic source file detection is under construction.\n";
	}
	for (int fn_i = 0; fn_i < fnames.size(); fn_i++) {
		cout << "mc: getting function declarations from `";
		cout << fnames[fn_i] << "`.\n";
		if (fnames.size() > 1)
			cout << "mc: file " << fn_i+1 << " of " << fnames.size() << ".\n";

		vector<string> funcdefs;
		get_func_defs(funcdefs, fnames[fn_i]);

		map<string, vector<string>> scopedecs;
		form_scoped_declarations(scopedecs, funcdefs);
	}
}

void env() {
	unordered_map<string, string> vfnconf;
	require(get_vfnmake_conf(vfnconf));

	cout << "\nvfnmake.conf:\n\n";
	for (const auto item : vfnconf) {
		cout << item.first << ": " << item.second << "\n";
	}
	cout << "\n";
}

int main(int argc, char *argv[]) {

	signal(SIGINT, signals_callback_handler);

	if (argc < 2)
		return help_message();
	for (int i = 2; i < argc; i++) {
		Argv.push_back(string(argv[i]));
		Args.append(string(" ").append(argv[i]));
	}

	if (!strcmp(argv[1], "help")) {
		help_message();
	}
	else if (!strcmp(argv[1], "makefile")) {
		VfnmakeSystemCall += Args;
		makefile();
	}
	else if (!strcmp(argv[1], "build")) {
		build();
	}
	else if (!strcmp(argv[1], "rebuild")) {
		rebuild();
	}
	else if (!strcmp(argv[1], "run")) {
		run();
	}
	else if (!strcmp(argv[1], "doc")) {
		doc();
	}
	else if (!strcmp(argv[1], "mkhead")) {
		mkhead();
	}
	else if (!strcmp(argv[1], "env")) {
		env();
	}
	else {
		// didn't recognize an argument!
		for (int i = 1; i < argc; i++) {
			cout << "mc: unrecognized argument: `" << argv[i] << "`.\n";
		}
		return help_message();
	}

	return 0;
}
