//
// mc.cpp
//
// manage c++ code
//
// Created by Daniel Kozitza
//

#include <csignal>
#include <cstdlib>
#include <dirent.h>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sys/ioctl.h>
#include <omp.h>
#include "commands.hpp"
#include "tools.hpp"
#include "sorters.hpp"

using namespace tools;

typedef map<string, string>::iterator map_iter;

void build(vector<string>& argv);
void cnt();
void env();
void makefile(vector<string>& argv);
void mkreadme(vector<string>& argv);
void rebuild(vector<string>& argv);
void run(vector<string>& argv);
void runtime(vector<string>& argv);
void runtimeavg(vector<string>& argv);

int main(int argc, char *argv[]) {
   vector<string> Argv(0);
   string         cmd_str;

   signal(SIGINT, signals_callback_handler);

   struct winsize ws;
   ioctl(0, TIOCGWINSZ, &ws);

   commands cmds;
   cmds.set_program_name(string(argv[0]));
   cmds.set_max_line_width(ws.ws_col);
   cmds.set_cmds_help(
      "\nmc is a tool for managing c++ source code.\n\n"
      "Usage:\n\n   mc command [arguments]\n");

   cmds.handle(
      "makefile",
      makefile,
      "Creates a make file by calling vfnmkmc [arguments].",
      "makefile [arguments]");
   cmds.handle(
      "build",
      build,
      "Calls vfnmkmc [arguments] (if needed) then calls make.",
      "build [arguments]");
   cmds.handle(
      "rebuild",
      rebuild,
      "Calls make clean, vfnmkmc [arguments] (if needed), then make.",
      "rebuild [arguments]");
   cmds.handle(
      "run",
      run,
      "Calls vfnmkmc (if needed), make, then ./program [arguments].",
      "run [arguments]");
   cmds.handle(
      "runtime",
      runtime,
      "Time the run command.",
      "runtime [arguments]");
   cmds.handle(
      "runtimeavg",
      runtimeavg,
      "Get the average time of multiple run executions.",
      "runtimeavg [RUNS=10, [arguments]]");
   cmds.handle(
      "env",
      env,
      "Displays the variables read from vfnmkmc.conf.",
      "env");
   cmds.handle(
      "mkreadme",
      mkreadme,
      "Make a README.md file from ./program [arguments].",
      "mkreadme");
   cmds.handle(
      "cnt",
      cnt,
      "Counts the number of lines in each of the source files.",
      "cnt");

   if (argc < 2)
      cmd_str = "help";
   else
      cmd_str = argv[1];

   for (int i = 2; i < argc; i++)
      Argv.push_back(string(argv[i]));

   cmds.run(cmd_str, Argv);

   return 0;
}

void makefile(vector<string>& argv) {
   string sys_call = "vfnmkmc";
   for (int i = 0; i < argv.size(); i++)
      sys_call += " \"" + argv[i] + "\"";

   cout << "mc::makefile: calling `" << sys_call << "`." << endl;
   require(system(sys_call.c_str()));
}

void build(vector<string>& argv) {
   map<string, string> vfnconf;
   if (!get_vfnmkmc_conf(vfnconf)) {
      makefile(argv);
      cout << "mc::build: reading vfnmkmc.conf." << endl;
      require(get_vfnmkmc_conf(vfnconf));
      vfnconf["src_files"] = get_src_files(vfnconf["src_directory"]);
      save_vfnmkmc_conf(vfnconf);
   }
   else {
      // vfnmkmc.conf was read, check if current source file listing matches
      // the one in vfnconf.
      string new_src_files = get_src_files(vfnconf["src_directory"]);
      if (vfnconf["src_files"] != new_src_files) {
         // if no match set new_src_files and call vfnmake
         vfnconf["src_files"] = new_src_files;
         save_vfnmkmc_conf(vfnconf);
         makefile(argv);
      }
   }

   cout << "mc::build: calling `make`." << endl;
   require(system("make"));
}

void rebuild(vector<string>& argv) {
   cout << "mc::rebuild: calling `make clean`.\n";
   require(system("make clean"));
   build(argv);
}

void run(vector<string>& argv) {
   vector<string> empty_v;
   build(empty_v);
   map<string, string> vfnconf;
   require(get_vfnmkmc_conf(vfnconf));

   string system_call = "./";
   system_call += vfnconf["name"];
   for (int i = 0; i < argv.size(); i++)
      system_call += " \"" + argv[i] + "\"";

   cout << "mc::run: calling `" << system_call << "`.\n";
   system(system_call.c_str());
}

void runtime(vector<string>& argv) {
   double start, end;
   vector<string> junk;
   build(junk);
   map<string, string> vfnconf;
   require(get_vfnmkmc_conf(vfnconf));

   string system_call = "./";
   system_call += vfnconf["name"];
   for (int i = 0; i < argv.size(); i++)
      system_call += " \"" + argv[i] + "\"";
   cout << "mc::runtime: calling `" << system_call << "`.\n";

   start = omp_get_wtime();
   system(system_call.c_str());
   end = omp_get_wtime();
   
   cout << "mc::runtime: execution time (seconds): `";
   cout << end - start << "`.\n";
}

void runtimeavg(vector<string>& argv) {
   double start, end, average = 0;
   int runs;

   vector<string> junk;
   build(junk);
   map<string, string> vfnconf;
   require(get_vfnmkmc_conf(vfnconf));
   if (argv.size() < 1)
      runs = 10;
   else
      runs = atoi(argv[0].c_str());

   string system_call = "./";
   system_call += vfnconf["name"];
   for (int z = 1; z < argv.size(); z++)
      system_call += " \"" + argv[z] + "\"";

   for(int j = 0; j < runs; ++j) {
      cout << "mc::runtimeavg: starting run " << j + 1 << "/" << runs << ".\n";
      cout << "mc::runtimeavg: calling `" << system_call << "`.\n";

      start = omp_get_wtime();
      system(system_call.c_str());
      end = omp_get_wtime();

      cout << "mc::runtimeavg: execution time for run " << j + 1 << "/";
      cout << runs << " (seconds): `" << end - start << "`.\n";

      if (average != 0)
         average = (average + (end - start)) / 2;
      else
         average = end - start;
   }

   cout << "mc::runtimeavg: execution time average after ";
   cout << runs << " runs (seconds): `";
   cout << average << "`.\n";
}

void env() {
   map<string, string> vfnconf;
   cout << "mc::env: reading vfnmkmc.conf." << endl;
   require(get_vfnmkmc_conf(vfnconf));

   cout << "\nvfnmkmc.conf:\n\n";
   //for (const auto item : vfnconf) {
   //   cout << item.first << ": " << item.second << "\n";
   //}

   for (map_iter it = vfnconf.begin(); it != vfnconf.end(); ++it)
      cout << it->first << ": " << it->second << "\n";

   cout << "\n";
}

void mkreadme(vector<string>& argv) {
   map<string, string> vfnconf;
   cout << "mc::mkreadme: reading vfnmkmc.conf." << endl;
   require(get_vfnmkmc_conf(vfnconf));

   string sys_call = "echo '# " + vfnconf["name"] + "' > README.md";
   cout << "mc::mkreadme: calling `" << sys_call << "`.\n";
   require(system(sys_call.c_str()));

   sys_call = "./" + vfnconf["name"];
   for (int i = 0; i < argv.size(); ++i)
      sys_call += " \"" + argv[i] + "\"";
   sys_call += " >> README.md";

   cout << "mc::mkreadme: calling `" << sys_call << "`.\n";
   system(sys_call.c_str());
}

void cnt() {
   map<string, string> vfnconf;
   vector<string> contents(0);
   unsigned int total_lines = 0;
   string src_dir;

   if (get_vfnmkmc_conf(vfnconf))
      src_dir = vfnconf["src_directory"];
   else
      src_dir = ".";

   if (!list_dir_r(src_dir, contents)) {
      cerr << "mc::cnt: vfnmkmc src_directory `" + src_dir;
      cerr << "` does not exist\n";
      return;
   }

   vector<string> new_contents(0);
   for (int i = 0; i < contents.size(); ++i) {
      if (pmatches(contents[i], "(\\.cpp|\\.c|\\.hpp|\\.h)$")) {
         new_contents.push_back(contents[i]);
      }
   }
   contents = new_contents;
   new_contents.clear();

   int longest = 0;
   for (int i = 0; i < contents.size(); ++i) {
      string fname = src_dir + "/" + contents[i];
      if (fname.size() > longest)
         longest = fname.size() + 1;
   }

   sorters::radix(contents);

   ifstream fh;
   for (int i = 0; i < contents.size(); ++i) {
      string fname = src_dir + "/" + contents[i];

      fh.open(fname.c_str(), ifstream::in);
      if (!fh.is_open()) {
         cout << "mc::cnt: could not open file: `" << fname << "`\n";
         continue;
      }

      char c;
      int file_lines = 0;
      while (fh.peek() != EOF) {
         fh.get(c);
         if (c == '\n')
            ++file_lines;
      }
      fh.close();
      total_lines += file_lines;

      fname += ":";
      cout << left << setw(longest) << fname;
      cout << " " << file_lines << endl;
   }

   cout << endl << left << setw(longest) << "total_loc:";
   cout << " " << total_lines << "\n\n";
}
