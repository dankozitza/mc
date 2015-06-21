//
// tools.cpp
// utility functions for mc
//
// Created by Daniel Kozitza
//

#include "tools.hpp"
#include <iostream>
#include <fstream>
#include <regex>

using namespace std;

void tools::get_vfnmake_conf(unordered_map<string, string>& config) {

	ifstream fh;
	fh.open("vfnmake.conf", ifstream::in);
	if (!fh.is_open()) {
		cout << "mc: could not open vfnmake.conf\n";
		return;
	}
	while(fh.peek() != EOF) {
		string line;
		getline(fh, line);

		string key = line;
		string val = line;

		if (line[0] == '#')
			continue;

		// get the key
		unsigned colon_i = key.find(":");
		if (colon_i == 0 || colon_i == string::npos)
			continue;
		key.resize(colon_i);

		// get the value
		val.replace(0, colon_i+1, "");
		while (val.find(" ") == 0) {
			val.replace(0, 1, "");
		}

		config[key] = val;
	}

	fh.close();
}

bool tools::matches(map<string, string>& results, string s, string str_re) {

	results = map<string, string>();

	smatch sm;
	regex re;
	try {
		regex rete (str_re.c_str());
		re.assign(rete);
	}
	catch (regex_error& e){
		cout << "tools::matches: regex returned error code: `";
	  	cout << e.code() << "` when evaluating expression: `" << str_re << "`\n";
		throw e;
	}

	try {
		if (!regex_match(s, sm, re))
			return false;

		for (unsigned i=0; i<sm.size(); i++) {
			char buffer [33];
			sprintf(buffer, "%d", i);
			results[buffer] = sm[i];
		}
	}
	catch (regex_error& e){
		cout << "tools::matches: regex_match returned error code: `";
		cout << e.code() << "` when called with s: `" << s;
		cout << "` and expression: `" << str_re << "`\n";
		return false;
	}

	return true;
}

void tools::test_matches() {

	map<string, string> m;
  	if (matches(m, "subject", "(sub)(.*)") == false)
		cout << "test failed!, first matches call returned false!\n";

	if (m["0"] != "subject")
		cout << "test failed!, match 0 [" << m["0"] << "] did not match!\n";

	if (m["1"] != "sub")
		cout << "test failed!, match 1 [" << m["1"] << "] did not match!\n";

	if (m["2"] != "ject")
		cout << "test failed!, match 2 [" << m["2"] << "] did not match!\n";

	if (matches(m, "a   ", "^(\\w+)\\s+(\\w*)$") == false)
		cout << "test failed!, second matches call returned false!\n";

	if (m["0"] != "a   ")
		cout << "test failed!, match 0 [" << m["0"] << "] did not match!\n";

	if (m["1"] != "a")
		cout << "test failed!, match 1 [" << m["1"] << "] did not match!\n";

	if (m["2"] != "")
		cout << "test failed!, match 2 [" << m["2"] << "] did not match!\n";
}
