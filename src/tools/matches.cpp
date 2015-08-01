//
// matches.cpp
// overloaded function that implements regex_match()
//
// Created by Daniel Kozitza
//

#include "../tools.hpp"
#include <iostream>

bool tools::matches(string s, string str_re) {
	smatch sm;
	return matches(sm, s, str_re);
}

// matches
//
// in case i don't want to include regex
//
bool tools::matches(string results[], string s, string str_re) {
	smatch sm;
	bool ret = matches(sm, s, str_re);
	if (!ret)
		return false;
	for (unsigned i=0; i<sm.size(); i++) {
		results[i] = sm[i];
	}
	return true;
}
bool tools::matches(vector<string>& results, string s, string str_re) {
	smatch sm;
	bool ret = matches(sm, s, str_re);
	if (!ret)
		return false;
	for (unsigned i=0; i<sm.size(); i++) {
		results.push_back(sm[i]);
	}
	return true;
}
//bool tools::matches(map<string, string>& results, string s, string str_re) {
//	smatch sm;
//	results = map<string, string>();
//	bool ret = matches(sm, s, str_re);
//	if (!ret)
//		return false;
//	for (unsigned i=0; i<sm.size(); i++) {
//		char si [33];
//		sprintf(si, "%d", i);
//		results[si] = sm[i];
//	}
//	return true;
//}

// matches
//
// saves the work of creating a regex object
//
// use R"(raw_characters)" for 'str_re' to avoid double escaping regex
//
bool tools::matches(smatch& sm, string s, string str_re) {

	sm = smatch();
	regex re;
	try {
		regex rete (str_re.c_str());//, regex_constants::basic);
		re.assign(rete);
	}
	catch (regex_error& e){
		cout << "tools::matches: regex returned error code: `";
		cout << e.code() << "` when evaluating expression: `" << str_re << "`\n";
		throw e;
	}

	try {
		//if (!regex_match(s, sm, re))
		//	return false;
		if (!regex_search(s, sm, re))
			return false;

	}
	catch (regex_error& e){
		cout << "tools::matches: regex_match returned error code: `";
		cout << e.code() << "` when called with s: `" << s;
		cout << "` and expression: `" << str_re << "`\n";
		return false;
	}

	return true;
}

bool tools::replace_first(string &s, string str_re, string fmt) {// yeah

	regex re;
	try {
		regex rete (str_re.c_str());//, regex_constants::format_first_only);
		re.assign(rete);
	}
	catch (regex_error& e){
		cout << "tools::replace_first: regex returned error code: `";
		cout << e.code() << "` when evaluating expression: `" << str_re << "`\n";
		throw e;
	}

	try {
		//if (!regex_match(s, sm, re))
		//	return false;
		regex_replace(s, re, fmt);
	}
	catch (regex_error& e){
		cout << "tools::replace_first: regex_match returned error code: `";
		cout << e.code() << "` when called with s: `" << s;
		cout << "` and expression: `" << str_re << "`\n";
		return false;
	}

	return true;
}

// test_matches
//
// A test for all the matches functions.
//
void tools::test_matches() {

	string am[10];
  	if (matches(am, "subject", "(sub)(.*)") == false)
		cout << "test failed!, first matches call returned false!\n";

	if (am[0] != "subject")
		cout << "test failed!, match 0 [" << am[0] << "] did not match!\n";

	if (am[1] != "sub")
		cout << "test failed!, match 1 [" << am[1] << "] did not match!\n";

	if (am[2] != "ject")
		cout << "test failed!, match 2 [" << am[2] << "] did not match!\n";

	if (matches(am, "a   ", "^(\\w+)\\s+(\\w*)$") == false)
		cout << "test failed!, second matches call returned false!\n";

	if (am[0] != "a   ")
		cout << "test failed!, match 0 [" << am[0] << "] did not match!\n";

	if (am[1] != "a")
		cout << "test failed!, match 1 [" << am[1] << "] did not match!\n";

	if (am[2] != "")
		cout << "test failed!, match 2 [" << am[2] << "] did not match!\n";

	vector<string> vm;
  	if (matches(vm, "subject", "(sub)(.*)") == false)
		cout << "test failed!, third matches call returned false!\n";

	if (vm[0] != "subject")
		cout << "test failed!, match 0 [" << vm[0] << "] did not match!\n";

	if (vm[1] != "sub")
		cout << "test failed!, match 1 [" << vm[1] << "] did not match!\n";

	if (vm[2] != "ject")
		cout << "test failed!, match 2 [" << vm[2] << "] did not match!\n";

	if (matches(vm, "a   ", "^(\\w+)\\s+(\\w*)$") == false)
		cout << "test failed!, fourth matches call returned false!\n";

	if (vm[0] != "a   ")
		cout << "test failed!, match 0 [" << vm[0] << "] did not match!\n";

	if (vm[1] != "a")
		cout << "test failed!, match 1 [" << vm[1] << "] did not match!\n";

	if (vm[2] != "")
		cout << "test failed!, match 2 [" << vm[2] << "] did not match!\n";


	smatch sm;
  	if (matches(sm, "subject", "(sub)(.*)") == false)
		cout << "test failed!, fifth matches call returned false!\n";

	if (sm[0] != "subject")
		cout << "test failed!, match 0 [" << sm[0] << "] did not match!\n";

	if (sm[1] != "sub")
		cout << "test failed!, match 1 [" << sm[1] << "] did not match!\n";

	if (sm[2] != "ject")
		cout << "test failed!, match 2 [" << sm[2] << "] did not match!\n";

	if (matches(sm, "a   ", "^(\\w+)\\s+(\\w*)$") == false)
		cout << "test failed!, sixth matches call returned false!\n";

	if (sm[0] != "a   ")
		cout << "test failed!, match 0 [" << sm[0] << "] did not match!\n";

	if (sm[1] != "a")
		cout << "test failed!, match 1 [" << sm[1] << "] did not match!\n";

	if (sm[2] != "")
		cout << "test failed!, match 2 [" << sm[2] << "] did not match!\n";

	//map<string, string> m;
  	//if (matches(m, "subject", "(sub)(.*)") == false)
	//	cout << "test failed!, seventh matches call returned false!\n";

	//if (m["0"] != "subject")
	//	cout << "test failed!, match 0 [" << m["0"] << "] did not match!\n";

	//if (m["1"] != "sub")
	//	cout << "test failed!, match 1 [" << m["1"] << "] did not match!\n";

	//if (m["2"] != "ject")
	//	cout << "test failed!, match 2 [" << m["2"] << "] did not match!\n";

	//if (matches(m, "a   ", "^(\\w+)\\s+(\\w*)$") == false)
	//	cout << "test failed!, eighth matches call returned false!\n";

	//if (m["0"] != "a   ")
	//	cout << "test failed!, match 0 [" << m["0"] << "] did not match!\n";

	//if (m["1"] != "a")
	//	cout << "test failed!, match 1 [" << m["1"] << "] did not match!\n";

	//if (m["2"] != "")
	//	cout << "test failed!, match 2 [" << m["2"] << "] did not match!\n";
}
