# 65f6f6650d611934e4e250f53df7cf7e
# Generated with vfnmkmc by the mc program.
PREFIX=/usr/local
CFLAGS=-O$(O)  -std=c++11
O=2
LFLAGS= -l gomp -l pcre2-8
OBJS=objs/commands.o objs/mc.o objs/radix.o objs/pcre2.o objs/strings.o objs/system.o objs/utils.o objs/vectors.o


.PHONY: all
all: objs mc

./mc:  $(OBJS)
	@ echo "    LINK ./mc"
	@ $(CXX) $(OBJS) -o "./mc" $(LFLAGS)

objs/commands.o: src/commands.cpp src/commands.hpp src/tools.hpp
	@ echo "    CXX  src/commands.cpp"
	@ $(CXX) $(CFLAGS) -c "src/commands.cpp" -o $@
objs/mc.o: src/mc.cpp src/commands.hpp src/tools.hpp src/sorters.hpp
	@ echo "    CXX  src/mc.cpp"
	@ $(CXX) $(CFLAGS) -c "src/mc.cpp" -o $@
objs/radix.o: src/sorters/radix.cpp src/sorters/../sorters.hpp
	@ echo "    CXX  src/sorters/radix.cpp"
	@ $(CXX) $(CFLAGS) -c "src/sorters/radix.cpp" -o $@
objs/pcre2.o: src/tools/pcre2.cpp src/tools/../tools.hpp
	@ echo "    CXX  src/tools/pcre2.cpp"
	@ $(CXX) $(CFLAGS) -c "src/tools/pcre2.cpp" -o $@
objs/strings.o: src/tools/strings.cpp src/tools/../tools.hpp
	@ echo "    CXX  src/tools/strings.cpp"
	@ $(CXX) $(CFLAGS) -c "src/tools/strings.cpp" -o $@
objs/system.o: src/tools/system.cpp src/tools/../tools.hpp
	@ echo "    CXX  src/tools/system.cpp"
	@ $(CXX) $(CFLAGS) -c "src/tools/system.cpp" -o $@
objs/utils.o: src/tools/utils.cpp src/tools/../sorters.hpp \
 src/tools/../tools.hpp
	@ echo "    CXX  src/tools/utils.cpp"
	@ $(CXX) $(CFLAGS) -c "src/tools/utils.cpp" -o $@
objs/vectors.o: src/tools/vectors.cpp src/tools/../tools.hpp
	@ echo "    CXX  src/tools/vectors.cpp"
	@ $(CXX) $(CFLAGS) -c "src/tools/vectors.cpp" -o $@

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
	@ cd deps/vfnmkmc; make clean; make uninstall;
	@ cd deps/pcre2; make clean; make uninstall;
	@ rm $(PREFIX)/bin/mc
	@ echo "[1;32m*[0m mc removed from $(PREFIX)/bin[0m"

deps:
	@ mkdir deps

deps/vfnmkmc: deps
	@ echo "    GET deps/vfnmkmc"
	@ cd deps; git clone https://github.com/dankozitza/vfnmkmc

deps/vfnmkmc/installed: deps/vfnmkmc
	@ echo "    MAKE INSTALL deps/vfnmkmc"
	@ cd deps/vfnmkmc; make install
	@ touch deps/vfnmkmc/installed

deps/pcre2: deps
	@ echo "    GET deps/pcre2"
	@ cd deps; wget ftp://ftp.csx.cam.ac.uk:21/pub/software/programming/pcre/pcre2-10.20.zip;
	@ cd deps; unzip pcre2-10.20.zip;
	@ cd deps; rm -f pcre2-10.20.zip;
	@ cd deps; mv pcre2-10.20 pcre2;

deps/pcre2/installed: deps/pcre2
	@ echo "    CONFIGURE deps/pcre2"
	@ echo "    MAKE INSTALL deps/pcre2"
	@ cd deps/pcre2; ./configure --prefix=/usr;
	@ cd deps/pcre2; make install;
	@ touch deps/pcre2/installed;


.PHONY: check-syntax
check-syntax:
	$(CXX) $(CFLAGS) -fsyntax-only -Wall -o /dev/null -S src/*
