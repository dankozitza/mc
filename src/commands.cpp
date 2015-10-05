//
// commands.cpp
//
// Created by Daniel Kozitza
//

#include <cassert>
#include <iostream>
#include <iomanip>
#include "commands.hpp"
#include "tools.hpp"

void dummy_func() {};
void dummy_func(vector<string>& j) {};

commands::commands() {
	cmd_name_width = 15;
}

// set_program_name
// 
// When this is set the commands object will act as if the command being
// run is from the command line arguments of a program named `pn`.
// 
// If set two things will happen:
//
//    1. the program name will be used in the main help output.
//
//    2. The program name will be prepended to usage strings.
//
void commands::set_program_name(string pn) {
	program_name = pn;
}

// set_cmds_help
// 
// This message is printed when run("help", Argv) is called. Before the list
// of available commands.
//
void commands::set_cmds_help(string msg) {
	cmds_help = msg;
}

// default_help
//
// This function is automatically called when the "help" command is run. If
// argv is empty it will print the help message for the entire commands object,
// `cmds_help`. This message is expected to explain what the situation is that
// requires user input and should include it's own usage. After the help message
// is printed a list of the available commands is printed.
//
// Ex:
//
//    commands cmds;
//    cmds.set_cmds_help(
//       "program_name is designed to ...\n"
//       "Usage: program_name command [arguments]\n");
//    cmds.handle("cmd_name", function_name, "synopsis", "usage");
//    cmds.run("help", Argv);
//
// If the argv vector has arguments in it the usage, synopsis, and description
// is printed for each command named in the arguments.
//
// TODO: fold lines at this->line_width when printing to cout
//
void commands::default_help(vector<string>& argv) {
	if (argv.size() == 0) {

		if (cmds_help != "")
			cout << cmds_help << endl;

		// print the list of available commands
		if (cmds.size() > 0) {
			cout << "Commands:\n\n";
			for (const auto c : cmds) {
				cout << "   " << left << setw(cmd_name_width) << c.first;
				cout << " - " << tools::fold(
					cmd_name_width + 6,
					80,
					c.second.synopsis) << endl;
			}

			cout << "\nUse \"";
			if (program_name != "")
				cout << program_name << " ";
			cout << "help [command]\" for more information about a command.\n\n";
		}
	}
	else {
		// print the usage, synopsis, and description of each argument
		for (int i = 0; i < argv.size(); i++) {
			if (cmds.find(argv[i]) != cmds.end()) {
				cout << "Usage: ";
				if (program_name != "")
					cout << program_name << " ";
				cout << cmds[argv[i]].usage << "\n\n";
				cout << cmds[argv[i]].synopsis << endl;
				if (cmds[argv[i]].description != "")
					cout << endl << cmds[argv[i]].description << endl;
				cout << endl;
			}
			else {
				// if the user calls help help
				if (argv[i] == "help") {
					vector<string> tmp;
					default_help(tmp);
				}
				else {
					cout << "Unknown command `" << argv[i] << "`. Try '";
					if (program_name != "")
						cout << program_name << " "; 
					cout << "help'.\n";
				}
			}
		}
	}
}

void commands::handle(
		string cmd,
		void (*func)(vector<string>&),
		string synopsis,
		string usage,
		string description) {
	
	Command tmp = {dummy_func, func, true, synopsis, usage, description};
	cmds[cmd]   = tmp;
}

void commands::handle(
		string cmd,
		void (*func)(),
		string synopsis,
		string usage,
		string description) {

	Command tmp = {func, dummy_func, false, synopsis, usage, description};
	cmds[cmd]   = tmp;
}

void commands::run(string cmd, vector<string>& arguments) {
	map<string, Command>::iterator it = cmds.find(cmd);

	// if help has been defined by the user do not run default_help
	if (cmd == "help" && it == cmds.end())
		default_help(arguments);

	else if (it != cmds.end())
		if (cmds[cmd].has_arguments)
			cmds[cmd].func_wa(arguments);
		else
			cmds[cmd].func_na();
}
