//
// tools.cpp
// utility functions for mc
//
// Created by Daniel Kozitza
//

#include "../tools.hpp"
#include <iostream>
#include <fstream>
#include <csignal>

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

// 
// SIGABRT: 6
// SIGFPE: 8
// SIGILL: 4
// SIGINT: 2
// SIGSEGV: 11
// SIGTERM: 15
//
vector<void (*)(int)> SigHandlers[16];

// signals_callback_handler
//
// the signal handler for all signal handlers. runs all the signal handlers in 
// the SigHandlers vector associated with that signal. use the signals function
// to add handlers to the SigHandlers array of vectors.
//
// function to be called when ctrl-c (SIGINT) signal is sent to process
//
void tools::signals_callback_handler(int signum) {

	cout << "\ntools::signals_callback_handler: caught signal `";
	cout << signum << "`.\n";

	if (signum < 0 || signum > 15) {
		cout << "	signal number out of bounds!!!\n";
		exit(signum);
	}

	// run all other handlers
	// this doesn't work if the handler exits
	for (const auto handler : SigHandlers[signum])
		handler(signum);

	// Terminate program
	exit(signum);
}

// signals
//
// replacement for signal that allows multiple callback handlers to be used for
// each signal. If a callback handler supplied to signals calls `exit`, all
// uncalled handlers in the SigHandlers vector will not be called.
// 
// signal(SIGINT, signals_callback_handler) must be called for program to exit
// on interrupt.
//
void tools::signals(int sig, void (*callback_func)(int)) {
	SigHandlers[sig].push_back(callback_func);
}

vector<string> Targets;

void destroy_targets() {
	for (const auto fname : Targets) {
		cout << "tools::destroy_targets: removing `" << fname << "`.\n";
		remove(fname.c_str());
	}
}

class suicide_bomber {
	string file_name;

	public:
		suicide_bomber(string fname) {
			cout << "tools::suicide_bomber: I WILL REMOVE `" << fname << "`!!!\n";
			file_name = fname;

			// for the destroy_targets signal handler
			Targets.push_back(fname);


			//signals(SIGINT, destroy_targets);
			atexit(destroy_targets);
		}
		void dud() {
			file_name = "";
		}
		void boom() {
			 if (file_name != "") {
				// boom only happens when the program exits nicely
				cout << "tools::suicide_bomber: DIE YOU GODLESS HEATHEN!!!!\n";
				remove(file_name.c_str());
				cout << "	removed `" <<  file_name << "`.\n";
			}
		}
		~suicide_bomber() {
			boom();
		}
};

bool CalledAtexit = false;

// add_documentation
//
// adds title to source files and descriptions to functions. if the first line
// of the file is not "//" it prepends a title to the file after prompting for
// a description of the file. if the line before a function declaration is not
// "//" it prepends a description of the function after prompting the user.
//
void tools::add_documentation(string fname) {

	string tfname = fname + "_mc_added_docs";
	string previous_line = "NO_PREVIOUS_LINE";
	string answer;
	bool made_change = false;

	ifstream ifh;
	ifh.open(fname, ifstream::in);
	if (!ifh.is_open()) {
		cout << "tools::add_documentation: couldn't open `" << fname << "`.\n";
		return;
	}

	ofstream ofh;
	ofh.open(tfname, ofstream::out);
	if (!ofh.is_open()) {
		cout << "tools::add_documentation: couldn't open `" << tfname << "`.\n";
		return;
	}

	// destroy_targets function will remove these at exit
	Targets.push_back(tfname);

	// we only want to call atexit once.
	if (!CalledAtexit) {
		CalledAtexit = true;
		atexit(destroy_targets);
	}

	int line_num = 1;
	while (ifh.peek() != EOF) {
		string line;
		getline(ifh, line);

		if (line_num == 1)  {
			vector<string> title;

			if (line != "//") {
				cout << "tools::add_documentation: making new title.\n";
				cout << "	lines will be broken at 80 automatically, \\n will be ";
				cout << "replaced with newlines.\n";
				cout << "	enter a description for the title:\n\n";
				string desc;
				getline(cin, desc);

				string short_fname = fname;
				string m[2];
				if (matches(m, fname, R"(/(.*?)$)")) {
					cout << "	short_fname: `" << m[1] << "`.\n";
					short_fname = m[1];
				}
				
				cout << "	adding title:\n\n";

				title.push_back("//");
				title.push_back("// " + short_fname);
				title.push_back("//");
				if (desc != "") {
					title.push_back("// " + desc); // TODO: break lines at 80
					title.push_back("//");
				}
				title.push_back("// Created by Daniel Kozitza");
				title.push_back("//");
				for (const auto item : title)
					cout << item << "\n";
				cout << "\n";
				for (const auto item : title)
					ofh << item << endl;
				made_change = true;
			}
			else {
				cout << "tools::add_documentation: file already has a title.\n";
			}
		}

		vector<string> m; // TODO: make sure m[1] matches all characters allowed
								//       in a c++ function name.
		if (matches(m, line, R"(^\w.*?(\w+)\(.*\)? *\{?)")) {
			// prefunc is the comment block that will be written before a function.
			vector<string> prefunc;

			if (previous_line != "//") {
				cout << "tools::add_documentation: found function declaration ";
				cout << "without documentation:\n";
				cout << "	" << fname << " line " << line_num << ":\n\n";
				cout << line << "\n\n";
				cout << "	add description? (Y/n): ";
				getline(cin, answer);
				if (answer != "n") {
					cout << "	lines will break at 80 automatically, \\n will be ";
					cout << "replaced with newlines.\n";
					cout << "	enter a description for the function `" << m[1];
					cout << "`:\n\n";
					string desc;
					getline(cin, desc);
					
					cout << "	adding description:\n\n";
					prefunc.push_back("// " + m[1]);
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
		return;
	}

	cout << "tools::add_documentation: finished!\n	new file is `" << tfname;
	cout << "`.\n\n";
	cout << "	overwrite existing file? (Y/n): ";

	getline(cin, answer);

	if (answer != "n") {
		cout << "	removing `" << fname << "`.\n";
		remove(fname.c_str());
		cout << "	renaming `" << tfname << "` to `" << fname << "`.\n";
		rename(tfname.c_str(), fname.c_str());
	}
	else {
		cout << "	overwrite canceled!\n";
	}
}

// get_func_defs
//
// This function populates the `definitions` vector with all of the file scope
// function definitions found in `fname`. the items inside the `definitions`
// vector will be the entire first line of the definition.
//
// eg: the `definitions` element produced from this function's definition
// would be:
//
// void tools::get_func_defs(vector<string>& definitions, string fname) {
//
void tools::get_func_defs(vector<string>& definitions, string fname) {

	ifstream ifh;
	ifh.open(fname, ifstream::in);
	if (!ifh.is_open()) {
		cout << "tools::get_func_defs: couldn't open `" << fname << "`.\n";
		return;
	}

	int line_num = 1;
	bool multi_line_def = false;
	while (ifh.peek() != EOF) {
		string line;
		getline(ifh, line);

		// TODO: get rid of comments and trailing spaces
		//string m[1];
		//if (matches(m, line, R"(^(.*)//)"))
		//	line = m[1];
		//
		//replace_first(line, R"(//)", "//_replacement_blarg_blarg");

		if (multi_line_def) {
//			cout << "tools::get_func_defs: matched line: `" << line << "`.\n";
			definitions[definitions.size()-1].append("\n" + line);
			if (matches(line, R"(\{)"))
				multi_line_def = false;
			line_num++;
			continue;
		}

		// TODO: make this work on multi-line definitions
		if (matches(line, R"(^\w.*?\w+\(.*\)? *\{?)")) {
//			cout << "tools::get_func_defs: matched line: `" << line << "`.\n";
			definitions.push_back(line);

			if (!matches(line, R"(\{)"))
				multi_line_def = true;
		}

		line_num++;
	}

	ifh.close();
}

void tools::form_scoped_declarations(
		map<string, vector<string>> &sd,
		vector<string> defs) {

	cout << "tools::form_scoped_declarations: testing:\n";
	for (const auto line : defs) {
		// here we only want definitions that have scope operators, we will use
		// the scope to find the block of code that the declaration belongs!
		string m[4];

		string nl = line;
		replace_all(nl, R"(\n)", "TEMP_NEWLINE_REPLACEMENT2534324567");
	
		// remove everything after the last parenthesis
		replace_first(nl, R"([^)]+$)", "");

		// add the semicolon
		nl += ";";

		if (matches(m, nl, R"(^(.*?)(\w+)\:\:(.*)$)")) {

			nl = m[1] + m[3];
			replace_all(nl, R"(TEMP_NEWLINE_REPLACEMENT2534324567)", "\n");

			cout << "	`" << m[2] << "`: `" << nl << "`.\n";

			sd[m[2]].push_back(nl);
		}
	}
}

void tools::update_namespaces(
		string namespace_name,
		vector<string> new_declarations,
		string header_fname) {

	cout << "tools::update_namespaces: testing\n";
}

// require
//
// overloaded function for exiting on failure with optional message.
// TODO: have this throw an exception instead.
//
// exit if sys_exit_val is not zero.
//
bool tools::require(int sys_exit_val, string msg)
{
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

void tools::get_includes(vector<string>& includes, string fname) {
	cout << "tools::get_includes: opening file `" << fname << "`.\n";

	ifstream ifh;
	ifh.open(fname, ifstream::in);
	if (!ifh.is_open()) {
		cout << "tools::get_includes: couldn't open `" << fname << "`.\n";
		return;
	}

	while (ifh.peek() != EOF) {
		string line;
		getline(ifh, line);

		string m[2];
		if (matches(m, line, R"(^#include \"(.*)\")")) {
			cout << "	matched line: `" << line << "`, m[1]: `" << m[1] << "`.\n";

			// make sure there's no duplicates
			bool dup = false;
			for (const auto item : includes) {
				if (item == m[1]) {
					cout << "	already have `" << m[1] << "`.\n";
					dup = true;
					break;
				}
			}

			if (!dup)
				includes.push_back(m[1]);
		}
	}

	ifh.close();
}
