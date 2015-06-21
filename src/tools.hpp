// tools.hpp
// utility functions for mc
//
// Created by Daniel Kozitza
//

#ifndef _TOOLS
#define _TOOLS

#include <map>
#include <unordered_map>

using namespace std;

namespace tools {
	void get_vfnmake_conf(unordered_map<string, string>& config);
	bool rm_ending_pair(string pair, string&s, int rm_number_pairs = 1);
	bool matches(map<string, string>& results, string s, string regex);
	void test_matches();
}

#endif
