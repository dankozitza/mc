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
	cout << "   dec      - Ensures that all the functions listed in the given\n";
	cout << "              source files are declared properly.\n";
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

void dec() {
	vector<string> fnames = Argv;

	if (fnames.size() == 0) {
		cout << "mc: automatic source file detection is under construction.\n";
	}

	map<string, vector<string>> scopedecs;
	// vector of all user defined includes found in fnames
	vector<string> includes; 

	for (int fn_i = 0; fn_i < fnames.size(); fn_i++) {
		cout << "mc: getting function declarations from `";
		cout << fnames[fn_i] << "`.\n";
		if (fnames.size() > 1)
			cout << "mc: file " << fn_i+1 << " of " << fnames.size() << ".\n";

		vector<string> funcdefs;
		get_func_defs(funcdefs, fnames[fn_i]);

		form_scoped_declarations(scopedecs, funcdefs);

		get_includes(includes, fnames[fn_i]);
	}

	//do this for each namespace found?
	for (const auto nsname : scopedecs) {

		cout << "mc::dec: searching for namespace definition called `";
		cout << nsname.first << "`.\n";

		// now scopedecs is pupulated with all the scoped declarations found in
		// the files fnames. Search for the header file by first checking the
		// user defined includes in the source files for the namespace definition.
		bool found_header_fname = false;
		string header_fname;

		// TODO: should try the src dir from all source files given
		string src_dir = fnames[0];
		replace_first(src_dir, R"([^/]+$)", "");

		for (const auto inc : includes) {
			string str_re = "^namespace " + nsname.first + "\\W";
			cout << "mc::dec: trying regex `" << str_re << "` in file `";
			cout << src_dir << inc << "`.\n";
			// TODO: make sure str_re is properly escaped
			if (find_in_file(str_re, src_dir + inc)) {
				cout << "	found the header file!\n";
				found_header_fname = true;
				header_fname = src_dir + inc;
				break;
			}
		}

		// TODO: finish all these methods of searching for the header_fname

		//if (!found_header_fname) {
		//	// here try to replace the extention of fnames with hpp then h
		//}

		//if (!found_header_fname) {
		//	// here try to replace the extention and move up one dir
		//}
		//
		//if (!found_header_fname) {
		//	// here try to look in the fnames files themselves
		//}
		//
		//if (!found_header_fname) {
		//	// here check every file found in the src dir
		//}
		//
		cout << "mc::dec: new_declarations:\n\n";
		for (const auto item : nsname.second)
			cout << item << endl;
	
		cout << endl;

		require(
			found_header_fname,
			"mc::dec: could not find namespace `" + nsname.first + "`.");

		update_namespace(nsname.first, nsname.second, header_fname);
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

	//commands cmds;
	//cmds.handle(
	//		"help",
	//		help_message,
	//		"displays this message",
	//		"USAGE: mc help [command]");
//},
	//	{"makefile", makefile},
	//	{"build", build},
	//	{"rebuild", rebuild},
	//	{"run", run},
	//	{"doc", doc},
	//	{"dec", dec},
	//	{"env", env}};

	if (argc < 2)
		return help_message();

	for (int i = 2; i < argc; i++) {
		Argv.push_back(string(argv[i]));
		Args.append(string(" ").append(argv[i]));
	}

	// make map of ints and use switch instead of if-else
	map<string, const int> commands = {
		{"help", 0},
		{"makefile", 1},
		{"build", 2},
		{"rebuild", 3},
		{"run", 4},
		{"doc", 5},
		{"dec", 6},
		{"env", 7}};

	switch (commands[string(argv[1])]) {
	case 0: help_message();
		break;
	case 1: VfnmakeSystemCall += Args; makefile();
		break;
	case 2: build();
		break;
	case 3: rebuild();
		break;
	case 4: run();
		break;
	case 5: doc();
		break;
	case 6: dec();
		break;
	case 7: env();
		break;
	default:
		// didn't recognize command!
		for (int i = 1; i < argc; i++) {
			cout << "mc: unrecognized command: `" << argv[i] << "`.\n";
		}
		return help_message();
	}

//	if (!strcmp(argv[1], "help")) {
//		help_message();
//	}
//	else if (!strcmp(argv[1], "makefile")) {
//		VfnmakeSystemCall += Args;
//		makefile();
//	}
//	else if (!strcmp(argv[1], "build")) {
//		build();
//	}
//	else if (!strcmp(argv[1], "rebuild")) {
//		rebuild();
//	}
//	else if (!strcmp(argv[1], "run")) {
//		run();
//	}
//	else if (!strcmp(argv[1], "doc")) {
//		doc();
//	}
//	else if (!strcmp(argv[1], "dec")) {
//		dec();
//	}
//	else if (!strcmp(argv[1], "env")) {
//		env();
//	}
//	else {
//		// didn't recognize command!
//		for (int i = 1; i < argc; i++) {
//			cout << "mc: unrecognized command: `" << argv[i] << "`.\n";
//		}
//		return help_message();
//	}

	return 0;
}
