# mc

mc is a tool for managing c++ source code.

###Usage:
```bash
   mc command [arguments]
```
###Commands:
```text
   build      - Calls vfnmake <arguments> then make.
   dec        - Ensures that all the functions listed in the given c++ source 
                files are declared properly.
   doc        - Parses c++ files adding documentation and prompting the user for
                function descriptions
   env        - Displays the variables read from vfnmake.conf.
   makefile   - Creates a make file by calling vfnmake <arguments>.
   mkreadme   - Make a README.md file from ./program <arguments>.
   rebuild    - Calls make clean, vfnmake <arguments>, then make.
   run        - Calls vfnmake, make, then ./program <arguments>.

Use "mc help [command]" for more information about a command.
```
