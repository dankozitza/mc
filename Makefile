# 60ffd7daff953f818706c184c0b655b1
PREFIX=/usr/local
CFLAGS=-O$(O)  -std=c++11
O=2
LFLAGS=
OBJS=objs/mc.o objs/commands.o objs/matches.o objs/tools.o


.PHONY: all
all: objs mc

./mc: deps/vfnmake/installed  $(OBJS)
	@ echo "    LINK ./mc"
	@ $(CXX) $(OBJS) -o "./mc" $(LFLAGS)

objs/mc.o: src/mc.cpp src/commands.hpp src/tools.hpp
	@ echo "    CXX  src/mc.cpp"
	@ $(CXX) $(CFLAGS) -c "src/mc.cpp" -o $@
objs/commands.o: src/commands.cpp src/commands.hpp
	@ echo "    CXX  src/commands.cpp"
	@ $(CXX) $(CFLAGS) -c "src/commands.cpp" -o $@
objs/matches.o: src/tools/matches.cpp src/tools/../tools.hpp
	@ echo "    CXX  src/tools/matches.cpp"
	@ $(CXX) $(CFLAGS) -c "src/tools/matches.cpp" -o $@
objs/tools.o: src/tools/tools.cpp src/tools/../tools.hpp
	@ echo "    CXX  src/tools/tools.cpp"
	@ $(CXX) $(CFLAGS) -c "src/tools/tools.cpp" -o $@

objs:
	@ mkdir "objs"

.PHONY: c clean
c: clean
clean:
	@ if [ -d "objs" ]; then rm -r "objs"; fi
	@ rm -f "./mc"
	@ echo "    CLEAN"
.PHONY: f fresh
f: fresh
fresh: clean
	@ make all --no-print-directory
.PHONY: r run
r: run
run: all
	@ ././mc

.PHONY: d debug
d: debug
debug: CFLAGS += -DDEBUG -g3 -Wall -Wextra
debug: O=0
debug: all

.PHONY: sc superclean
sc: superclean
superclean: clean
	@ rm -rf "deps"
	@ echo "    SUPERCLEAN"

.PHONY: install
install: all
	@ install -D -m 755 mc $(PREFIX)/bin/mc
	@ echo "[1;32m*[0m mc installed in $(PREFIX)/bin"
	@ echo "[1muse \`make uninstall\` to remove mc[0m"

.PHONY: uninstall
uninstall:
	@ cd deps/vfnmake; make clean; make uninstall;
	@ rm $(PREFIX)/bin/mc
	@ echo "[1;32m*[0m mc removed from $(PREFIX)/bin[0m"

deps:
	@ mkdir deps

deps/vfnmake: deps
	@ echo "    GET deps/vfnmake"
	@ cd deps; git clone https://github.com/dankozitza/vfnmake

deps/vfnmake/installed: deps/vfnmake
	@ echo "    MAKE INSTALL deps/vfnmake"
	@ cd deps/vfnmake; make install
	@ touch deps/vfnmake/installed

.PHONY: check-syntax
check-syntax:
	$(CXX) $(CFLAGS) -fsyntax-only -Wall -o /dev/null -S src/*
