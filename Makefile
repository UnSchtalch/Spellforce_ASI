DLL_OBJS = obj/sf_asi.o obj/dllmain.o
DLL_OBJS1 = obj/sf_asi.o obj/worker.o
CC = g++
RC = windres


DLL_CFLAGS = -O2 -g -std=c++11 ${WARNS} -Iinclude -D ADD_EXPORTS -fpermissive

DLL_LDFLAGS = -shared -s -Wl,--subsystem,windows,--out-implib,lib/buildext.a

.PHONY: all clean

all: bin/buildext.asi bin/worker.asi

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

obj/worker.o: src/elvenworker.cpp src/asi/sf_asi.h | obj
	${CC} ${DLL_CFLAGS} -c "$<" -o "$@"

bin/buildext.asi: ${DLL_OBJS}| bin lib
	${CC} -o "$@" ${DLL_OBJS} ${DLL_LDFLAGS}

bin/worker.asi: ${DLL_OBJS1}| bin lib
	${CC} -o "$@" ${DLL_OBJS1} ${DLL_LDFLAGS}