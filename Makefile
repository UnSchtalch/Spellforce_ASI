DLL_OBJS = obj/sf_asi.o obj/dllmain.o

CC = g++
RC = windres
DLL = buildext.asi

DLL_CFLAGS = -O2 -g -std=c++11 ${WARNS} -Iinclude -D ADD_EXPORTS -fpermissive

DLL_LDFLAGS = -shared -s -Wl,--subsystem,windows,--out-implib,lib/buildext.a

.PHONY: all clean

all: bin/${DLL}

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

bin/${DLL}: ${DLL_OBJS} | bin lib
	${CC} -o "$@" ${DLL_OBJS} ${DLL_LDFLAGS}