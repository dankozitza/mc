//
// mc.cpp
//
// manage c++ code
//
// Created by Daniel Kozitza
//

#include <csignal>
#include <sys/ioctl.h>
#include "commands.hpp"
#include "tools.hpp"

using namespace tools;

void build(vector<string>& argv);
void dec(vector<string>& argv);
void doc(vector<string>& argv);
void env();
void makefile(vector<string>& argv);
void mkreadme();
void rebuild(vector<string>& argv);
void run(vector<string>& argv);

int main(int argc, char *argv[]) {
	vector<string> Argv;
	string         cmd_str;

	signal(SIGINT, signals_callback_handler);

	struct winsize ws;
	ioctl(0, TIOCGWINSZ, &ws);

	commands cmds;
	cmds.set_program_name(string(argv[0]));
	cmds.set_max_line_width(ws.ws_col);
	cmds.set_cmds_help(
		"\nmc is a tool for managing c++ source code.\n\n"
		"Usage:\n\n   mc command [arguments]\n");

	cmds.handle(
			"makefile",
			makefile,
			"Creates a make file by calling vfnmake [arguments].",
			"makefile [arguments]");
	cmds.handle(
			"build",
			build,
			"Calls vfnmake [arguments] then make.",
			"build [arguments]");
	cmds.handle(
			"rebuild",
			rebuild,
			"Calls make clean, vfnmake [arguments], then make.",
			"rebuild [arguments]");
	cmds.handle(
			"run",
			run,
			"Calls vfnmake, make, then ./program [arguments].",
			"run [arguments]");
	cmds.handle(
			"doc",
			doc,
			"Parses c++ files adding documentation and "
			"prompting the user for function descriptions.",
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
			"Make a README.md file from ./program [arguments].",
			"mkreadme");

	if (argc < 2)
		cmd_str = "help";
	else
		cmd_str = argv[1];

	for (int i = 2; i < argc; i++)
		Argv.push_back(string(argv[i]));

	cmds.run(cmd_str, Argv);

	return 0;
}

void makefile(vector<string>& argv) {
	string sys_call = "vfnmake";
	for (int i = 0; i < argv.size(); i++)
		sys_call += " " + argv[i];

	cout << "mc::makefile: calling `" << sys_call << "`.\n";
	require(system(sys_call.c_str()));
}

void build(vector<string>& argv) {
	makefile(argv);
	cout << "mc::build: calling `make`.\n";
	require(system("make"));
}

void rebuild(vector<string>& argv) {
	cout << "mc::rebuild: calling `make clean`.\n";
	require(system("make clean"));
	build(argv);
}

void run(vector<string>& argv) {
	string args;
	for (int i = 0; i < argv.size(); i++)
		args += " " + argv[i];

	vector<string> junk;
	build(junk);

	map<string, string> vfnconf;
	require(get_vfnmake_conf(vfnconf));

	string named_prog_call = "./";
	named_prog_call += vfnconf["name"];
	named_prog_call += args;
	cout << "mc::run: calling `" << named_prog_call << "`.\n\n";
	system(named_prog_call.c_str());
}

void doc(vector<string>& argv) {
	vector<string> fnames = argv;

	if (fnames.size() == 0) {
		// get .cpp file names by recursing through vfnconf["src_directory"]
		cout << "mc::doc: auto source file detection is under construction.\n";
	}
	for (int fn_i = 0; fn_i < fnames.size(); fn_i++) {
		cout << "mc::doc: adding documentation to `" << fnames[fn_i] << "`.\n";
		if (fnames.size() > 1)
			cout << "mc::doc: file " << fn_i+1 << " of " << fnames.size() << ".\n";
		add_documentation(fnames[fn_i]);
	}
}

void dec(vector<string>& argv) {
	vector<string> fnames = argv;

	if (fnames.size() == 0) {
		cout << "mc::dec: auto source file detection is under construction.\n";
	}

	map<string, vector<string>> scopedecs;
	// vector of all user defined includes found in fnames
	vector<string> includes; 

	for (int fn_i = 0; fn_i < fnames.size(); fn_i++) {
		cout << "mc::dec: getting function declarations from `";
		cout << fnames[fn_i] << "`.\n";
		if (fnames.size() > 1)
			cout << "mc::dec: file " << fn_i+1 << " of " << fnames.size() << ".\n";

		vector<string> funcdefs;
		get_func_defs(funcdefs, fnames[fn_i]);

		cout << "mc::dec: checking funcdefs:\n\n";
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
	map<string, string> vfnconf;
	require(get_vfnmake_conf(vfnconf));

	string sys_call = "echo '# " + vfnconf["name"] + "' > README.md";
	cout << "mc::mkreadme: calling `" << sys_call << "`.\n";
	require(system(sys_call.c_str()));

	sys_call = "./" + vfnconf["name"];
	for (int i = 0; i < vfnconf["name"].size(); ++i)
		sys_call += " " + vfnconf["name"][i];
	sys_call += " >> README.md";

	cout << "mc::mkreadme: calling `" << sys_call << "`.\n";
	system(sys_call.c_str());
}
