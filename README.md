# mc

mc is a tool for managing c++ source code.

### Usage:

```bash
mc command [arguments]
```

### Commands:

```text
   help     - Show this help message
   makefile - Creates a make file by calling vfnmake <arguments>
   build    - Calls vfnmake <arguments> then make.
   rebuild  - Calls make clean, vfnmake <arguments>, then make
   run      - Calls vfnmake, make, then ./program <arguments>
   env      - Displays the variables read from vfnmake.conf
```

### Install

```bash
make install
```
