#ifndef _COMMANDS
#define _COMMANDS

// map is a a map of strings paired to functions that take strings of vectors
// void funk(vector<string> blarg);
// commands cmds = {"funk_command": funk};

class commands {
	map<string, void (*)(vector<string>)
