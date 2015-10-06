//
// system.cpp
//
// Created by Daniel Kozitza
//

#include "../tools.hpp"
#include <csignal>

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
