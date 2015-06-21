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

using namespace std;

string args;
string vfnmake_call = "vfnmake";

int helpMessage() {
	cout << "\nmc is a tool for managing c++ source code.\n\n";
	cout << "Usage:\n\n   mc command [arguments]\n\n";
	cout << "Commands:\n\n";
	cout << "   help     - Show this help message\n";
	cout << "   makefile - Creates a make file by calling vfnmake <arguments>\n";
	cout << "   build    - Calls vfnmake <arguments> then make.\n";
	cout << "   rebuild  - Calls make clean, vfnmake <arguments>, then make\n";
	cout << "   run      - Calls vfnmake, make, then ./program <arguments>\n";
	cout << "\n";
	return EXIT_FAILURE;
}

void exitIfNotZero(int i) {
	if (i != 0) {
		cout << "mc: got return value " << i << "\n";
		exit(EXIT_FAILURE);
	}
}

int makefile() {
	cout << "mc: calling `" << vfnmake_call << "`\n";
	exitIfNotZero(system(vfnmake_call.c_str()));
	return 0;
}

int build() {
	exitIfNotZero(makefile());
	exitIfNotZero(system("make"));
	return 0;
}

int rebuild() {
	exitIfNotZero(system("make clean"));
	exitIfNotZero(build());
	return 0;
}

int run () {
	exitIfNotZero(build());

	unordered_map<string, string> vfnconf;
	tools::get_vfnmake_conf(vfnconf);

	string named_prog_call = "./";
	named_prog_call += vfnconf["name"];
	named_prog_call += args;
	cout << "mc: calling `" << named_prog_call << "`\n";
	exitIfNotZero(system(named_prog_call.c_str()));
	return 0;
}

int main(int argc, char *argv[]) {

	if (argc < 2)
		return helpMessage();

	for (int i = 2; i < argc; i++)
		args.append(string(" ").append(argv[i]));

	if (!strcmp(argv[1], "help")) {
		helpMessage();
		return 0;
	}

	if (!strcmp(argv[1], "makefile")) {
		vfnmake_call += args;
		return makefile();
	}

	if (!strcmp(argv[1], "build"))
		return build();

	if (!strcmp(argv[1], "rebuild"))
		return rebuild();

	if (!strcmp(argv[1], "run"))
		return run();

	// didn't recognize an argument!
	for (int i = 1; i < argc; i++) {
		cout << "\nmc: unrecognized argument: " << argv[i] << "\n";
	}
	return helpMessage();
}
