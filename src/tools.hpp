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
	void only_in_header();
	bool get_vfnmake_conf(unordered_map<string, string>& config);
	void signals_callback_handler(int signum);
	void signals(int sig, void (*callback_func)(int));
	void add_documentation(string fname);
	void get_func_defs(vector<string>& definitions, string fname);
	void form_scoped_declarations(
			map<string, vector<string>> &sd,
			vector<string> defs);
	void update_namespace(
			string namespace_name,
			vector<string> new_declarations,
			string fname);
	bool require(int sys_exit_val, string msg = "");
	bool require(bool func_return_val, string msg = "");
	bool matches(string s, string str_re);
	bool matches(string results[], string s, string str_re);
	bool matches(vector<string>& results, string s, string str_re);
	//bool matches(map<string, string>& results, string s, string regex);
	bool matches(smatch& sm, string s, string str_re);
	bool replace_first(string &s, string str_re, string fmt);
	bool replace_all(
			string &s,
			string str_re,
			string fmt,
			regex_constants::match_flag_type mf = regex_constants::match_default);

	void test_matches();
	void test_replace();
	void get_includes(vector<string>& includes, string fname);
	bool find_in_file(string str_re, string fname);
}

#endif
