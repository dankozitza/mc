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
#include "commands.hpp"
#include "tools.hpp"

using namespace tools;

string         Args;
vector<string> Argv;
string         VfnmakeSystemCall = "vfnmake";

void makefile();
void build();
void rebuild();
void run();
void doc();
void dec();
void env();
void mkreadme();

int main(int argc, char *argv[]) {

	signal(SIGINT, signals_callback_handler);

	commands cmds;
	cmds.set_program_name(string(argv[0]));
	cmds.set_cmds_help(
		"\nmc is a tool for managing c++ source code.\n\n"
		"Usage:\n\n   mc command [arguments]\n");

	cmds.handle(
			"makefile",
			makefile,
			"Creates a make file by calling vfnmake <arguments>.",
			"makefile [arguments]");
	cmds.handle(
			"build",
			build,
			"Calls vfnmake <arguments> then make.",
			"build [arguments]");
	cmds.handle(
			"rebuild",
			rebuild,
			"Calls make clean, vfnmake <arguments>, then make.",
			"rebuild [arguments]");
	cmds.handle(
			"run",
			run,
			"Calls vfnmake, make, then ./program <arguments>.",
			"run [arguments]");
	cmds.handle(
			"doc",
			doc,
			"Parses c++ files adding documentation and "
			"prompting the user for function descriptions",
			"doc CPP_FILE");
	cmds.handle(
			"dec",
			dec,
			"Ensures that all the functions listed in the given c++ "
			"source files are declared properly.",
			"dec CPP_FILE");
	cmds.handle(
			"env",
			env,
			"Displays the variables read from vfnmake.conf.",
			"env");
	cmds.handle(
			"mkreadme",
			mkreadme,
			"Make a README.md file from ./program <arguments>.",
			"mkreadme");

	if (argc < 2) {
		cmds.run("help", Argv);
		return 0;
	}

	for (int i = 2; i < argc; i++) {
		Argv.push_back(string(argv[i]));
		Args.append(string(" ").append(argv[i]));
	}

	cmds.run(string(argv[1]), Argv);

	return 0;
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

	map<string, string> vfnconf;
	require(get_vfnmake_conf(vfnconf));

	string named_prog_call = "./";
	named_prog_call += vfnconf["name"];
	named_prog_call += Args;
	cout << "mc: calling `" << named_prog_call << "`.\n\n";
	system(named_prog_call.c_str());
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

		cout << "mc: checking funcdefs:\n\n";
		for (const auto item : funcdefs)
			cout << item << endl;
		cout << endl;

		form_scoped_declarations(scopedecs, funcdefs);

		get_includes(includes, fnames[fn_i]);
	}

	// do this for each namespace found?
	// ns_c_pair.first is the name of the namespace or class.
	// ns_c_pair.second is a vector of strings containing the declarations
	// created by form_scoped_declarations.
	for (const auto ns_c_pair : scopedecs) {

		cout << "mc::dec: searching for namespace definition called `";
		cout << ns_c_pair.first << "`.\n";

		// now scopedecs is pupulated with all the scoped declarations found in
		// the files fnames. Search for the header file by first checking the
		// user defined includes in the source files for the namespace definition.
		bool found_header_fname = false;
		string header_fname;

		// TODO: should try the src dir from all source files given
		string src_dir = fnames[0];
		replace_first(src_dir, R"([^/]+$)", "");

		for (const auto inc : includes) {
			// TODO: make sure this matches what we want
			string str_re = "^(namespace|class) " + ns_c_pair.first + "\\W";
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

		cout << "mc::dec: new_declarations:\n\n";
		for (const auto item : ns_c_pair.second)
			cout << item << endl;
	
		cout << endl;

		require(
			found_header_fname,
			"mc::dec: could not find namespace `" + ns_c_pair.first + "`.");

		update_ns_or_class(ns_c_pair.first, ns_c_pair.second, header_fname);
	}
}

void env() {
	map<string, string> vfnconf;
	require(get_vfnmake_conf(vfnconf));

	cout << "\nvfnmake.conf:\n\n";
	for (const auto item : vfnconf) {
		cout << item.first << ": " << item.second << "\n";
	}
	cout << "\n";
}

void mkreadme() {
	system("mc help >> GENERATED_README.md");
}
