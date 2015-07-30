//
// tools.hpp
// utility functions for mc
//
// Created by Daniel Kozitza
//

#ifndef _TOOLS
#define _TOOLS

#include <map>
#include <unordered_map>
#include <regex>

using namespace std;

namespace tools {
	bool get_vfnmake_conf(unordered_map<string, string>& config);
	void signals_callback_handler(int signum);
	void signals(int sig, void (*callback_func)(int));
	void add_documentation(string fname);
	void func_declarations(vector<string>& declarations, string fname);
	bool require(int sys_exit_val, string msg = "");
	bool require(bool func_return_val, string msg = "");
	bool matches(string s, string str_re);
	bool matches(string results[], string s, string regex);
	bool matches(vector<string>& results, string s, string regex);
	//bool matches(map<string, string>& results, string s, string regex);
	bool matches(smatch& sm, string s, string regex);
	void test_matches();
}

#endif
