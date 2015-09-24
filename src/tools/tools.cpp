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
	cout << signum << "`\n";

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
		cout << "tools::destroy_targets: removing `" << fname << "`\n";
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
				cout << "	removed `" <<  file_name << "`\n";
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
		cout << "tools::add_documentation: couldn't open `" << fname << "`\n";
		return;
	}

	ofstream ofh;
	ofh.open(tfname, ofstream::out);
	if (!ofh.is_open()) {
		cout << "tools::add_documentation: couldn't open `" << tfname << "`\n";
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
					cout << "	short_fname: `" << m[1] << "`\n";
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
	cout << "`\n\n";
	cout << "	overwrite existing file? [Y/n]: ";

	getline(cin, answer);

	if (answer != "n") {
		cout << "	removing `" << fname << "`\n";
		remove(fname.c_str());
		cout << "	renaming `" << tfname << "` to `" << fname << "`\n";
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
		cout << "tools::get_func_defs: couldn't open `" << fname << "`\n";
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
//			cout << "tools::get_func_defs: matched line: `" << line << "`\n";
			definitions[definitions.size()-1].append("\n" + line);
			if (matches(line, R"(\{)"))
				multi_line_def = false;
			line_num++;
			continue;
		}

		// try to match a function definition
		if (matches(line, R"(^\w.*?\w+\(.*\)? *\{?)") ||
					matches(line, R"(^\w.*?operator.*?\(.*\)? *\{?)")) {
//			cout << "tools::get_func_defs: matched line: `" << line << "`\n";
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

			cout << "	`" << m[2] << "`: `" << nl << "`\n";

			sd[m[2]].push_back(nl);
		}
	}
}

// update_ns_or_class
//
//	updates the namespace or class `ns_c_name` with the declarations found in the
//	`new_declarations` vector in the file found at `fname`
//
//	   - reads fname retreiving an old list of declarations
//    - compares the old list with the new list
//    - 
//
void tools::update_ns_or_class(
		string ns_c_name,
		vector<string> new_declarations,
		string fname) {

	// declarations found in the existing header
	vector<string> old_declarations;

	// declarations only found in the new vector
	// assume these are to be added to the header
	vector<string> only_in_new;

 	// declarations only found in the old vector
	// prompt the user to decide on action
	vector<string> only_in_old;

	// temporary filename for making updated file
	string tfname = fname + "_mc_updated_decs";
	bool made_change = false;
	string indent = "	";

	cout << "tools::update_ns_or_class: opening file `" << fname << "`\n";

	ifstream ifh;
	ifh.open(fname, ifstream::in);
	if (!ifh.is_open()) {
		cout << "tools::update_ns_or_class: couldn't open `" << fname << "`\n";
		return;
	}

	cout << "tools::update_ns_or_class: opening file `" << tfname << "`\n";

	ofstream ofh;
	ofh.open(tfname, ofstream::out);
	if (!ofh.is_open()) {
		cout << "tools::update_ns_or_class: couldn't open `" << tfname << "`\n";
		return;
	}

	// destroy_targets function will remove this at exit
	Targets.push_back(tfname);

	// only want to call atexit once.
	if (!CalledAtexit) {
		CalledAtexit = true;
		atexit(destroy_targets);
	}

	while (ifh.peek() != EOF) {
		string line;
		getline(ifh, line);

		// create a regular expression to match the namespace or class definition
		string ns_c_def_re = "^(namespace|class) " + ns_c_name + "\\W";

		// TODO: perhaps to remove the problem of scope i could have a loop here
		// that attempts to match ns_c_def_re and on fail it enters other scopes.

		// begining of namespace block
		if (matches(line, ns_c_def_re)) {

			// this is a vector of the entire namespace or class. function
			// declarations are replaced with a placeholder.
			vector<string> ns_block;
			ns_block.push_back(line);

			// now collect all the old declarations
			bool multi_line_dec = false;
			while (ifh.peek() != EOF && !matches(line, R"(})")) {
				getline(ifh, line);

				// skip comments
				if (matches(line, R"(^\s*//)")) {
					ns_block.push_back(line);
					continue;
				}

				if (multi_line_dec) {
					// get the remaining lines of the declaration
					string new_line = line;
					replace_all(new_line, R"(^	)", "");
					old_declarations[old_declarations.size()-1].append("\n" + line);
					
					if (matches(line, R"(;)")) {
						multi_line_dec = false;
					}
					continue;
				}

				// try to match a function declaration
				string m[3];
				if (matches(m, line, R"(^.*\s(.*)\()")) {

					//cout << "	found declaration: `" << line << "`\n";
					//cout << "		m[1]: `" << m[1] << "`\n";

					// get the indent and set 
					string m2[4];
					if (matches(m, line, R"(^(\s+)(.*)$)")) {
						indent = m[1];
						old_declarations.push_back(m[2]);
					}
					else {
						old_declarations.push_back(line);
					}

					// put placeholder in ns_block
					char num[33];
					sprintf(num, "%d", old_declarations.size() - 1);
					ns_block.push_back("placeholder " + string(num));

					if (!matches(line, R"(;)"))
						multi_line_dec = true;

				}
				else {
					// if the line is not a function declaration just put it in
					ns_block.push_back(line);
				}
			}

			for (auto &od : old_declarations)
				replace_all(od, R"(\n	)", "\n");

			// begin comparing lists

			// get the only_in_old list and replace placeholders for declarations
			// that are found in both new and old lists
			for (int odi = 0; odi < old_declarations.size(); odi++) {
				string old_dec = old_declarations[odi];
				bool found = false;
				for (const auto new_dec : new_declarations) {

					if (old_dec == new_dec) {
						//cout << "\n	old_dec: `" << old_dec << "`\n	new_dec: `";
						//cout << new_dec << "`\n";
						//cout << "	^^^match!\n";
						found = true;
						// find the placeholder to replace since this declaration is
						// in both new and old lists.
						for (int i = 0; i < ns_block.size(); i++) {
							char re[30];
							sprintf(re, "^placeholder %d$", odi);
							if (matches(ns_block[i], string(re))) {

								string s = indent + old_dec;
								replace_all(s, R"(\n)", "\n" + indent);
								ns_block[i] = s;
								break;
							}
						}
						break;
					}
				}
				if (!found) {
					only_in_old.push_back(old_dec);
				}
			}

//			cout << "	only_in_old:\n\n";
//			for (const auto item : only_in_old)
//				cout << item << endl;
//			cout << endl;

			// get the only_in_new list
			for (const auto new_dec : new_declarations) {
				bool found = false;
				for (const auto old_dec : old_declarations) {
					if (new_dec == old_dec)
						found = true;
				}
				if (!found) {
					only_in_new.push_back(new_dec);
				}
			}

//			cout << "	only_in_new:\n\n";
//			for (const auto item : only_in_new)
//				cout << item << endl;
//			cout << endl;

			// prompt user about what to do for each item in the ony_in_old vector
			// remove replaced declarations from the new_declarations vector
			for (const auto od : only_in_old) {

				string nice_od = od;
				replace_all(
						nice_od,
						R"(\n)",
						"`\n		                    `");

				cout << "	namespace has extra declaration:";
				cout << "\n\n		                    `";
				cout << nice_od << "`\n\n	commands:\n\n";
				cout << "		l - leave it (default)\n";
				cout << "		r - remove it\n";
				for (int i = 0; i < only_in_new.size(); i++) {
					cout << "		" << i << " - replace it with `";

					string nice_nd = only_in_new[i];
					replace_all(nice_nd, R"(\n)", "`\n		                    `");
					cout << nice_nd << "`\n";
				}

				// get old_declarations index: `odi`
				int odi = -1;
				for (int i = 0; i < old_declarations.size(); i++) {
					if (old_declarations[i] == od) {
						odi = i;
						break;
					}
				}

				// find the placeholder index for this declaration
				int phi = -1;
				for (int i = 0; i < ns_block.size(); i++) {
					char re[30];
					sprintf(re, "^placeholder %d$", odi);
					if (matches(ns_block[i], string(re))) {
						phi = i;
						break;
					}
				}

				// interact with user and replace the placeholder in ns_block with
				// the old declaration, a new declaration, or remove the line.
				while (true) {

					cout << "\n	enter command: ";
					string opt;
					getline(cin, opt);

					if (opt == "l" || opt == "") {
						// use od and leave the file as is
						string s = indent + od;
						replace_all(s, R"(\n)", "\n" + indent);
						ns_block[phi] = s;
						break;
					}
					else if (opt == "r") {
						made_change = true;
						// remove the placeholder line entirely
						ns_block.erase(ns_block.begin()+phi);
						break;
					}
					else if (matches(opt, R"(^\d+$)")
							&& atoi(opt.c_str()) < only_in_new.size()
							&& atoi(opt.c_str()) > -1) {
						made_change = true;
						// replace it with only_in_new[opt]
						string s = indent + only_in_new[atoi(opt.c_str())];
						replace_all(s, "\n", "\n" + indent);
						ns_block[phi] = s;

						// remove the declaration from the only_in_new vector
						only_in_new.erase(only_in_new.begin()+atoi(opt.c_str()));
						break;
					}

					cout << "	invalid option!";
				}
			}

			// prompt the user about what to do with remaining only_in_new
			while (only_in_new.size() > 0) {
				cout << "	there are items left in the only_in_new vector:\n\n";
				cout << "	commands:\n\n";
				cout << "		i - ignore these items and leave them";
				cout << " out of the namespace\n";
				for (int i = 0; i < only_in_new.size(); i++) {
					string nice_nd = only_in_new[i];
					replace_all(nice_nd, R"(\n)", "`\n		        `");
					cout << "		" << i << " - add `" << nice_nd;
					cout << "` to the namespace\n";
				}

				while (true) {
					cout << "\n	enter command: ";
					string opt;
					getline(cin, opt);

					if (opt == "i") {
						// remove the remaining only_in_new declarations
						only_in_new.clear();
						break;
					}
					else if (matches(opt, R"(^\d+$)")
							&& atoi(opt.c_str()) < only_in_new.size()
							&& atoi(opt.c_str()) > -1) {
						made_change = true;
						// add only_in_new[opt] to ns_block
						string s = indent + only_in_new[atoi(opt.c_str())];
						replace_all(s, R"(\n)", "\n" + indent);
						ns_block.push_back(s);

						// remove the declaration from the only_in_new vector
						only_in_new.erase(only_in_new.begin()+atoi(opt.c_str()));
						break;
					}

					cout << "	invalid option!";
				}
			}

			// write the new namespace definition to ofh
			for (const auto item : ns_block)
				ofh << item << endl;
	
		} // end of namespace block
		else {
			// copy the lines to the new file
			ofh << line << endl;
		}
	}

	ofh.close();
	ifh.close();

	if (!made_change) {
		cout << "tools::update_ns_or_class: no changes made!\n";
		return;
	}

	// TODO: handle situations where the declaration has default values defined

	string cmd_str = "diff " + fname + " " + tfname;
	cout << "tools::update_ns_or_class: calling `" << cmd_str << "`:\n\n";
	system(cmd_str.c_str());
	cout << "\ntools::update_ns_or_class: finished!\n	new file is `" << tfname;
	cout << "`\n\n";
	cout << "	overwrite existing file? [Y/n]: ";

	cmd_str = "y";
	getline(cin, cmd_str);

	if (cmd_str != "n") {
		cout << "	removing `" << fname << "`\n";
		remove(fname.c_str());
		cout << "	renaming `" << tfname << "` to `" << fname << "`\n";
		rename(tfname.c_str(), fname.c_str());
	}
	else {
		cout << "	overwrite canceled!\n";
	}
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
	cout << "tools::get_includes: opening file `" << fname << "`\n";

	ifstream ifh;
	ifh.open(fname, ifstream::in);
	if (!ifh.is_open()) {
		cout << "tools::get_includes: couldn't open `" << fname << "`\n";
		return;
	}

	while (ifh.peek() != EOF) {
		string line;
		getline(ifh, line);

		string m[2];
		if (matches(m, line, R"(^#include \"(.*)\")")) {
			cout << "	matched line: `" << line << "`, m[1]: `" << m[1] << "`\n";

			// make sure there's no duplicates
			bool dup = false;
			for (const auto item : includes) {
				if (item == m[1]) {
					cout << "	already have `" << m[1] << "`\n";
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
