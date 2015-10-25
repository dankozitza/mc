//
// tools.hpp
// utility functions for mc
//
// Created by Daniel Kozitza
//

#ifndef _TOOLS
#define _TOOLS

#include <iostream>
#include <map>
#include <regex>
#include <vector>

using namespace std;

namespace tools {
   bool get_vfnmake_conf(map<string, string>& config);
   void signals_callback_handler(int signum);
   void signals(int sig, void (*callback_func)(int));
   void get_func_defs(vector<string>& definitions, string fname);
   void form_scoped_declarations(
         map<string, vector<string>> &sd,
         vector<string> defs);
   void update_ns_or_class(
         string ns_c_name,
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
   string fold(int indent_width, int max_line_width, string s);
   istream& operator>>(istream& is, vector<string>& v);
   ostream& operator<<(ostream& os, vector<string>& v);
   void vectors_test();
   bool dir_exists(string dir_name);
   bool list_dir(string dir_name, vector<string>& contents);
   bool list_dir_r(string dir_name, vector<string>& contents);
   bool list_dir_r(
         string dir_name,
         vector<string>& contents,
         string prefix);


   // doesn't work
   //
   //   vector<string> contents = generate();
   //   vector<size_t> pos = tools::radix_sort<128>(contents);
   //   for (auto i : pos)
   //      std::cout << contents[i] << std::endl;
   //
   //template <typename E>
   //size_t bin(const E& elem, size_t digit);
   //
   //template <size_t R, typename C, typename P>
   //void radix_sort(P& pos, const C& data, size_t digit);
   //
   //template <size_t R, typename C>
   //vector<size_t> radix_sort(const C& data);

}

#endif
