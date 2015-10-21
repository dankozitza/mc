# mc

mc is a tool for managing c++ source code.

###Usage:

```bash
mc command [arguments]
```

###Commands:

```text
   build      - Calls vfnmake [arguments] then make.
   cnt        - Counts the number of lines in each of the source files.
   dec        - Ensures that all the functions listed in the given c++
                source files are declared properly.
   env        - Displays the variables read from vfnmake.conf.
   makefile   - Creates a make file by calling vfnmake [arguments].
   mkreadme   - Make a README.md file from ./program [arguments].
   rebuild    - Calls make clean, vfnmake [arguments], then make.
   run        - Calls vfnmake, make, then ./program [arguments].
```

Use `mc help [command]` for more information about a command.

### Install:

```bash
make install
```
