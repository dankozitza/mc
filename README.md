# mc

mc is a tool for managing c++ source code.

### Usage:

```bash
mc command [arguments]
```

### Commands:

```text
   build      - Calls vfnmkmc [arguments] (if needed) then calls make.
   cnt        - Counts the number of lines in every source file.
   env        - Displays the variables read from vfnmkmc.conf.
   makefile   - Make a Makefile by calling vfnmkmc [arguments].
   mkreadme   - Make a README.md file from ./program [arguments].
   rebuild    - Calls make clean, vfnmkmc [arguments] (if needed), then make.
   run        - Calls vfnmkmc (if needed), make, then ./program [arguments].
   runtime    - Time the run command.
   runtimeavg - Get the average time of multiple run executions.
```

Use `mc help [command]` for more information about a command.

### Install:

```bash
make install
```

### Dependencies

```text
c++11, gomp
```
