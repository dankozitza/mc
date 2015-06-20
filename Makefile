PREFIX=/usr/local
CFLAGS=-O$(O) 
O=2
LFLAGS=
OBJS=objs/mc.o


.PHONY: all
all: deps/vfnmake/vfnmake.conf objs mc

deps:
	@ mkdir deps

deps/vfnmake: deps
	@ echo "    GET deps/vfnmake"
	@ cd deps; git clone https://github.com/dankozitza/vfnmake

deps/vfnmake/vfnmake.conf: deps/vfnmake
	@ echo "    MAKE INSTALL deps/vfnmake"
	@ cd deps/vfnmake; make install

./mc: $(OBJS)
	@ echo "    LINK ./mc"
	@ $(CXX) $(OBJS) -o "./mc" $(LFLAGS)

objs/mc.o: src/mc.cpp
	@ echo "    CXX  src/mc.cpp"
	@ $(CXX) $(CFLAGS) -c "src/mc.cpp" -o $@

objs:
	@ mkdir "objs"
.PHONY: c clean
c: clean
clean:
	@ if [ -d "objs" ]; then rm -r "objs"; fi
	@ rm -f "./mc"
	@ echo "    CLEAN"

.PHONY: sc super-clean
sc: super-clean
super-clean: clean
	@ rm -rf "deps"

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

.PHONY: check-syntax
check-syntax:
	$(CXX) $(CFLAGS) -fsyntax-only -Wall -o /dev/null -S $(CHK_SOURCES)

.PHONY: install
install: all
	@ install -D -m 755 mc $(PREFIX)/bin/mc
	@ echo "[1;32m*[0m mc installed in $(PREFIX)/bin"
	@ echo "[1muse \`make uninstall\` to remove mc[0m"

.PHONY: uninstall
uninstall:
	@ cd deps/vfnmake; make clean; make uninstall
	@ rm $(PREFIX)/bin/mc
	@ echo "[1;32m*[0m mc removed from $(PREFIX)/bin"
