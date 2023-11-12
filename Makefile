BUILDEX_OBJS = obj/sf_asi.o obj/dllmain.o
NEWUPGD_OBJ = obj/sf_asi.o obj/new_upgrades.o
CC = g++
RC = windres


DLL_CFLAGS = -O2 -g -std=c++11 ${WARNS} -Iinclude -D ADD_EXPORTS -fpermissive
DLL1_LDFLAGS = -shared -s -Wl,--subsystem,windows,--out-implib,lib/buildext.a
DLL2_LDFLAGS = -shared -s -Wl,--subsystem,windows,--out-implib,lib/new_upgrades.a

.PHONY: all clean

all: bin/buildext.asi bin/new_upgrades.asi
clean:
	if exist bin\* del /q bin\*
	if exist lib\* del /q lib\*
	if exist obj\* del /q obj\*

bin lib obj:
	@if not exist "$@" mkdir "$@"

obj/sf_asi.o: src/asi/sf_asi.cpp src/asi/sf_asi.h | obj
	${CC} ${DLL_CFLAGS} -c "$<" -o "$@"

obj/dllmain.o: src/dllmain.cpp src/asi/sf_asi.h | obj
	${CC} ${DLL_CFLAGS} -c "$<" -o "$@"

obj/new_upgrades.o: src/new_upgrades.cpp src/asi/sf_asi.h | obj
	${CC} ${DLL_CFLAGS} -c "$<" -o "$@"

bin/new_upgrades.asi: ${NEWUPGD_OBJ}| bin lib
	${CC} -o "$@" ${NEWUPGD_OBJ} ${DLL2_LDFLAGS}


bin/buildext.asi: ${BUILDEX_OBJS}| bin lib
	${CC} -o "$@" ${BUILDEX_OBJS} ${DLL1_LDFLAGS}