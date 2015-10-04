//
// commands.cpp
//
// Created by Daniel Kozitza
//

#include <cassert>
#include <iostream>
#include <iomanip>
#include "commands.hpp"

void dummy_func() {};
void dummy_func(vector<string>& j) {};

commands::commands() {
	cmd_name_width = 20;
}

void commands::set_cmds_help(string msg) {
	cmds_help = msg;
}

// TODO: fold lines at this->line_width when printing to cout
void commands::default_help(vector<string>& argv) {
	if (argv.size() == 0) {

		if (cmds_help != "")
			cout << cmds_help << endl;

		if (cmds.size() > 0) {
			cout << "Commands:\n\n";
			cout << "   " << left << setw(cmd_name_width) << "help";
			cout << " - Show this help message. You can also use help [command] to get more information on another command.\n";
			for (const auto c : cmds) {
				cout << "   " << left << setw(cmd_name_width) << c.first;
				cout << " - " << c.second.synopsis << endl;
			}
			cout << endl;
		}
	}
	else {
		for (int i = 0; i < argv.size(); i++) {
			if (cmds.find(argv[i]) != cmds.end()) {
				cout << cmds[argv[i]].usage << "\n\n";
				cout << cmds[argv[i]].synopsis << endl;
				if (cmds[argv[i]].description != "")
					cout << endl << cmds[argv[i]].description << endl;
			}
			else {
				// if the user calls help help
				if (argv[i] == "help") {
					vector<string> tmp;
					default_help(tmp);
				}
				else
					cout << "unknown command `" << argv[i] << "`\n";
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
