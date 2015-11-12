//
// utils.cpp
//
// utility functions for mc
//
// Created by Daniel Kozitza
//

#include <fstream>
#include "../sorters.hpp"
#include "../tools.hpp"

bool CalledAtexit = false;

// get_vfnmkmc_conf
//
// Reads vnfmake.conf variables into 'config'.
//
bool tools::get_vfnmkmc_conf(map<string, string>& config) {

   ifstream fh;
   fh.open("vfnmkmc.conf", ifstream::in);
   if (!fh.is_open()) {
      cout << "tools::get_vfnmkmc_conf: couldn't open vfnmkmc.conf.\n";
      return false;
   }
   while(fh.peek() != EOF) {
      string line;
      getline(fh, line);

      string m[3];
      if (matches(m, line, R"(^\s*([^:]*):\s*(.*)$)"))
         config[m[1]] = m[2];
   }

   fh.close();
   return true;
}

bool tools::save_vfnmkmc_conf(map<string, string>& config) {
   FILE * pFile;
   pFile = fopen("vfnmkmc.conf", "w");
   for (auto& pair : config) {
      string tmp = pair.first + ":";
      fprintf(pFile, "%-20s%s\n", tmp.c_str(), pair.second.c_str());
   }

   fclose(pFile);
   return true;
}

vector<string> Targets;

void destroy_targets() {
   for (const auto fname : Targets) {
      cout << "tools::destroy_targets: removing `" << fname << "`\n";
      remove(fname.c_str());
   }
}

// get_func_defs
//
// This function populates the `definitions` vector with all of the file scope
// function definitions found in `fname`. the items inside the `definitions`
// vector will be the entire first line of the definition.
//
// eg: the `definitions` element produced from this function's definition
// would be:
//
// void tools::get_func_defs(vector<string>& definitions, string fname) {
//
void tools::get_func_defs(vector<string>& definitions, string fname) {

   ifstream ifh;
   ifh.open(fname, ifstream::in);
   if (!ifh.is_open()) {
      cout << "tools::get_func_defs: couldn't open `" << fname << "`\n";
      return;
   }

   int line_num = 1;
   bool multi_line_def = false;
   while (ifh.peek() != EOF) {
      string line;
      getline(ifh, line);

      // TODO: get rid of comments and trailing spaces
      //string m[1];
      //if (matches(m, line, R"(^(.*)//)"))
      //   line = m[1];
      //
      //replace_first(line, R"(//)", "//_replacement_blarg_blarg");

      if (multi_line_def) {
//         cout << "tools::get_func_defs: matched line: `" << line << "`\n";
         definitions[definitions.size()-1].append("\n" + line);
         if (matches(line, R"(\{)"))
            multi_line_def = false;
         line_num++;
         continue;
      }

      // try to match a function definition
      if (matches(line, R"(^\w.*?\w+\(.*\)? *\{?)") ||
               matches(line, R"(^\w.*?operator.*?\(.*\)? *\{?)")) {
//         cout << "tools::get_func_defs: matched line: `" << line << "`\n";
         definitions.push_back(line);

         if (!matches(line, R"(\{)"))
            multi_line_def = true;
      }

      line_num++;
   }

   ifh.close();
}

void tools::form_scoped_declarations(
      map<string, vector<string>> &sd,
      vector<string> defs) {

   cout << "tools::form_scoped_declarations: testing:\n";
   for (const auto line : defs) {
      // here we only want definitions that have scope operators, we will use
      // the scope to find the block of code that the declaration belongs!
      string m[4];

      string nl = line;
      replace_all(nl, "\n", "TEMP_NEWLINE_REPLACEMENT2534324567");
   
      // remove everything after the last parenthesis
      replace_first(nl, R"([^)]+$)", "");

      // add the semicolon
      nl += ";";

      if (matches(m, nl, R"(^(.*?)(\w+)\:\:(.*)$)")) {

         nl = m[1] + m[3];
         replace_all(nl, R"(TEMP_NEWLINE_REPLACEMENT2534324567)", "\n");

         cout << "   `" << m[2] << "`: `" << nl << "`\n";

         sd[m[2]].push_back(nl);
      }
   }
}

// update_ns_or_class
//
//   updates the namespace or class `ns_c_name` with the declarations found in the
//   `new_declarations` vector in the file found at `fname`
//
//      - reads fname retreiving an old list of declarations
//    - compares the old list with the new list
//    - 
//
void tools::update_ns_or_class(
      string ns_c_name,
      vector<string> new_declarations,
      string fname) {

   // declarations found in the existing header
   vector<string> old_declarations;

   // declarations only found in the new vector
   // assume these are to be added to the header
   vector<string> only_in_new;

    // declarations only found in the old vector
   // prompt the user to decide on action
   vector<string> only_in_old;

   // temporary filename for making updated file
   string tfname = fname + "_mc_updated_decs";
   bool made_change = false;
   string indent = "   ";

   cout << "tools::update_ns_or_class: opening file `" << fname << "`\n";

   ifstream ifh;
   ifh.open(fname, ifstream::in);
   if (!ifh.is_open()) {
      cout << "tools::update_ns_or_class: couldn't open `" << fname << "`\n";
      return;
   }

   cout << "tools::update_ns_or_class: opening file `" << tfname << "`\n";

   ofstream ofh;
   ofh.open(tfname, ofstream::out);
   if (!ofh.is_open()) {
      cout << "tools::update_ns_or_class: couldn't open `" << tfname << "`\n";
      return;
   }

   // destroy_targets function will remove this at exit
   Targets.push_back(tfname);

   // only want to call atexit once.
   if (!CalledAtexit) {
      CalledAtexit = true;
      atexit(destroy_targets);
   }

   while (ifh.peek() != EOF) {
      string line;
      getline(ifh, line);

      // create a regular expression to match the namespace or class definition
      string ns_c_def_re = "^(namespace|class) " + ns_c_name + "\\W";

      // TODO: perhaps to remove the problem of scope i could have a loop here
      // that attempts to match ns_c_def_re and on fail it enters other scopes.

      // begining of namespace block
      if (matches(line, ns_c_def_re)) {

         // this is a vector of the entire namespace or class. function
         // declarations are replaced with a placeholder.
         vector<string> ns_block;
         ns_block.push_back(line);

         // now collect all the old declarations
         bool multi_line_dec = false;
         while (ifh.peek() != EOF && !matches(line, R"(})")) {
            getline(ifh, line);

            // skip comments
            if (matches(line, R"(^\s*//)")) {
               ns_block.push_back(line);
               continue;
            }

            if (multi_line_dec) {
               // get the remaining lines of the declaration
               string new_line = line;
               replace_all(new_line, R"(^   )", "");
               old_declarations[old_declarations.size()-1].append("\n" + line);
               
               if (matches(line, R"(;)")) {
                  multi_line_dec = false;
               }
               continue;
            }

            // try to match a function declaration
            string m[3];
            if (matches(m, line, R"(^.*\s(.*)\()")) {

               //cout << "   found declaration: `" << line << "`\n";
               //cout << "      m[1]: `" << m[1] << "`\n";

               // get the indent and set 
               string m2[4];
               if (matches(m, line, R"(^(\s+)(.*)$)")) {
                  indent = m[1];
                  old_declarations.push_back(m[2]);
               }
               else {
                  old_declarations.push_back(line);
               }

               // put placeholder in ns_block
               char num[33];
               sprintf(num, "%d", old_declarations.size() - 1);
               ns_block.push_back("placeholder " + string(num));

               if (!matches(line, R"(;)"))
                  multi_line_dec = true;

            }
            else {
               // if the line is not a function declaration just put it in
               ns_block.push_back(line);
            }
         }

         for (auto &od : old_declarations)
            replace_all(od, R"(\n   )", "\n");

         // begin comparing lists

         // get the only_in_old list and replace placeholders for declarations
         // that are found in both new and old lists
         for (int odi = 0; odi < old_declarations.size(); odi++) {
            string old_dec = old_declarations[odi];
            bool found = false;
            for (const auto new_dec : new_declarations) {

               if (old_dec == new_dec) {
                  //cout << "\n   old_dec: `" << old_dec << "`\n   new_dec: `";
                  //cout << new_dec << "`\n";
                  //cout << "   ^^^match!\n";
                  found = true;
                  // find the placeholder to replace since this declaration is
                  // in both new and old lists.
                  for (int i = 0; i < ns_block.size(); i++) {
                     char re[30];
                     sprintf(re, "^placeholder %d$", odi);
                     if (matches(ns_block[i], string(re))) {

                        string s = indent + old_dec;
                        replace_all(s, R"(\n)", "\n" + indent);
                        ns_block[i] = s;
                        break;
                     }
                  }
                  break;
               }
            }
            if (!found) {
               only_in_old.push_back(old_dec);
            }
         }

//         cout << "   only_in_old:\n\n";
//         for (const auto item : only_in_old)
//            cout << item << endl;
//         cout << endl;

         // get the only_in_new list
         for (const auto new_dec : new_declarations) {
            bool found = false;
            for (const auto old_dec : old_declarations) {
               if (new_dec == old_dec)
                  found = true;
            }
            if (!found) {
               only_in_new.push_back(new_dec);
            }
         }

//         cout << "   only_in_new:\n\n";
//         for (const auto item : only_in_new)
//            cout << item << endl;
//         cout << endl;

         // prompt user about what to do for each item in the ony_in_old vector
         // remove replaced declarations from the new_declarations vector
         for (const auto od : only_in_old) {

            string nice_od = od;
            replace_all(
                  nice_od,
                  R"(\n)",
                  "`\n                          `");

            cout << "   namespace has extra declaration:";
            cout << "\n\n                          `";
            cout << nice_od << "`\n\n   commands:\n\n";
            cout << "      l - leave it (default)\n";
            cout << "      r - remove it\n";
            for (int i = 0; i < only_in_new.size(); i++) {
               cout << "      " << i << " - replace it with `";

               string nice_nd = only_in_new[i];
               replace_all(nice_nd, R"(\n)", "`\n                          `");
               cout << nice_nd << "`\n";
            }

            // get old_declarations index: `odi`
            int odi = -1;
            for (int i = 0; i < old_declarations.size(); i++) {
               if (old_declarations[i] == od) {
                  odi = i;
                  break;
               }
            }

            // find the placeholder index for this declaration
            int phi = -1;
            for (int i = 0; i < ns_block.size(); i++) {
               char re[30];
               sprintf(re, "^placeholder %d$", odi);
               if (matches(ns_block[i], string(re))) {
                  phi = i;
                  break;
               }
            }

            // interact with user and replace the placeholder in ns_block with
            // the old declaration, a new declaration, or remove the line.
            while (true) {

               cout << "\n   enter command: ";
               string opt;
               getline(cin, opt);

               if (opt == "l" || opt == "") {
                  // use od and leave the file as is
                  string s = indent + od;
                  replace_all(s, R"(\n)", "\n" + indent);
                  ns_block[phi] = s;
                  break;
               }
               else if (opt == "r") {
                  made_change = true;
                  // remove the placeholder line entirely
                  ns_block.erase(ns_block.begin()+phi);
                  break;
               }
               else if (matches(opt, R"(^\d+$)")
                     && atoi(opt.c_str()) < only_in_new.size()
                     && atoi(opt.c_str()) > -1) {
                  made_change = true;
                  // replace it with only_in_new[opt]
                  string s = indent + only_in_new[atoi(opt.c_str())];
                  replace_all(s, "\n", "\n" + indent);
                  ns_block[phi] = s;

                  // remove the declaration from the only_in_new vector
                  only_in_new.erase(only_in_new.begin()+atoi(opt.c_str()));
                  break;
               }

               cout << "   invalid option!";
            }
         }

         // prompt the user about what to do with remaining only_in_new
         while (only_in_new.size() > 0) {
            cout << "   there are items left in the only_in_new vector:\n\n";
            cout << "   commands:\n\n";
            cout << "      i - ignore these items and leave them";
            cout << " out of the namespace\n";
            for (int i = 0; i < only_in_new.size(); i++) {
               string nice_nd = only_in_new[i];
               replace_all(nice_nd, R"(\n)", "`\n              `");
               cout << "      " << i << " - add `" << nice_nd;
               cout << "` to the namespace\n";
            }

            while (true) {
               cout << "\n   enter command: ";
               string opt;
               getline(cin, opt);

               if (opt == "i") {
                  // remove the remaining only_in_new declarations
                  only_in_new.clear();
                  break;
               }
               else if (matches(opt, R"(^\d+$)")
                     && atoi(opt.c_str()) < only_in_new.size()
                     && atoi(opt.c_str()) > -1) {
                  made_change = true;
                  // add only_in_new[opt] to ns_block
                  string s = indent + only_in_new[atoi(opt.c_str())];
                  replace_all(s, R"(\n)", "\n" + indent);

                  // TODO: this needs to insert s before the last item
                  ns_block.push_back(s);

                  // remove the declaration from the only_in_new vector
                  only_in_new.erase(only_in_new.begin()+atoi(opt.c_str()));
                  break;
               }

               cout << "   invalid option!";
            }
         }

         // write the new namespace definition to ofh
         for (const auto item : ns_block)
            ofh << item << endl;
   
      } // end of namespace block
      else {
         // copy the lines to the new file
         ofh << line << endl;
      }
   }

   ofh.close();
   ifh.close();

   if (!made_change) {
      cout << "tools::update_ns_or_class: no changes made!\n";
      return;
   }

   // TODO: handle situations where the declaration has default values defined

   string cmd_str = "diff " + fname + " " + tfname;
   cout << "tools::update_ns_or_class: calling `" << cmd_str << "`:\n\n";
   system(cmd_str.c_str());
   cout << "\ntools::update_ns_or_class: finished!\n   new file is `" << tfname;
   cout << "`\n\n";
   cout << "   overwrite existing file? [Y/n]: ";

   cmd_str = "y";
   getline(cin, cmd_str);

   if (cmd_str != "n") {
      cout << "   removing `" << fname << "`\n";
      remove(fname.c_str());
      cout << "   renaming `" << tfname << "` to `" << fname << "`\n";
      rename(tfname.c_str(), fname.c_str());
   }
   else {
      cout << "   overwrite canceled!\n";
   }
}

void tools::get_includes(vector<string>& includes, string fname) {
   cout << "tools::get_includes: opening file `" << fname << "`\n";

   ifstream ifh;
   ifh.open(fname, ifstream::in);
   if (!ifh.is_open()) {
      cout << "tools::get_includes: couldn't open `" << fname << "`\n";
      return;
   }

   while (ifh.peek() != EOF) {
      string line;
      getline(ifh, line);

      string m[2];
      if (matches(m, line, R"(^#include \"(.*)\")")) {
         cout << "   matched line: `" << line << "`, m[1]: `" << m[1] << "`\n";

         // make sure there's no duplicates
         bool dup = false;
         for (const auto item : includes) {
            if (item == m[1]) {
               cout << "   already have `" << m[1] << "`\n";
               dup = true;
               break;
            }
         }

         if (!dup)
            includes.push_back(m[1]);
      }
   }

   ifh.close();
}

string tools::get_src_files(string src_dir) {
   string src_files;
   vector<string> files;
   require(list_dir_r(src_dir, files));

   sorters::radix(files);
   if (files.size() > 0) {
      for (int i = 0; i < files.size() - 1; ++i) {
         if (matches(files[i], R"((\.cpp|\.c|\.hpp|\.h)$)"))
            src_files += files[i] + " ";
      }
      if (matches(files[files.size() - 1], R"((\.cpp|\.c|\.hpp|\.h)$)"))
         src_files += files[files.size() - 1];
   }

   return src_files;
}
