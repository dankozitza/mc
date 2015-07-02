//
// mc.cpp
// manage c++ code
//
// Created by Daniel Kozitza
//

#include <iostream>
#include <cstdlib>
#include <string.h>
#include "tools.hpp"

using namespace tools;

string Args;
string VfnmakeSystemCall = "vfnmake";

int help_message() {
	cout << "\nmc is a tool for managing c++ source code.\n\n";
	cout << "Usage:\n\n   mc command [arguments]\n\n";
	cout << "Commands:\n\n";
	cout << "   help     - Show this help message\n";
	cout << "   makefile - Creates a make file by calling vfnmake <arguments>\n";
	cout << "   build    - Calls vfnmake <arguments> then make\n";
	cout << "   rebuild  - Calls make clean, vfnmake <arguments>, then make\n";
	cout << "   run      - Calls vfnmake, make, then ./program <arguments>\n";
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

	if (argc < 2)
		return help_message();

	for (int i = 2; i < argc; i++)
		Args.append(string(" ").append(argv[i]));

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
	else if (!strcmp(argv[1], "env")) {
		env();
	}

	// didn't recognize an argument!
	for (int i = 1; i < argc; i++) {
		cout << "mc: unrecognized argument: `" << argv[i] << "`.\n";
	}
	return help_message();
}
