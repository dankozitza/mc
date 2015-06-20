//
// mc.cpp
// manage c++ code
//
// Created by Daniel Kozitza
//

#include <iostream>
#include <cstdlib>
#include <string.h>

using namespace std;

int helpMessage() {
	cout << "\nmc is a tool for managing c++ source code.\n\n";
	cout << "Usage:\n\n   mc [command]\n\n";
	cout << "Commands:\n\n";
	cout << "   help     - show this help message\n";
	cout << "   makefile - creates a make file with vfnmake\n";
	cout << "   build    - runs vfnmake then make\n";
	cout << "   rebuild  - runs make clean, vfnmake, then make\n";
	cout << "   run      - runs vfnmake then make run\n";
	cout << "\n";
	return EXIT_FAILURE;
}

void exitIfNotZero(int i) {
	if (i != 0) {
		cout << "mc: got return value " << i;
	   cout << "! you may need to modify/remove vfnmake.conf\n";
		exit(EXIT_FAILURE);
	}
}

int makefile() {
	exitIfNotZero(system("vfnmake"));
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
	// TODO: have mc read vfnmake.conf to get the name and other info
	exitIfNotZero(system("make run"));
	return 0;
}

int main(int argc, char *argv[]) {

	if (argc < 2)
		return helpMessage();

	if (!strcmp(argv[1], "help")) {
		helpMessage();
		return 0;
	}

	if (!strcmp(argv[1], "makefile"))
		return makefile();

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
