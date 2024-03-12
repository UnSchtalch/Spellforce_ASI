BUILDEX_OBJS = obj/sf_asi.o obj/dllmain.o
BUILDEXP_OBJS = obj/sf_asi.o obj/buildings_expanded.o
NEWUPGD_OBJ = obj/sf_asi.o obj/new_upgrades.o
ELVEN_OBJ = obj/sf_asi.o obj/elvenworker.o
CC = g++
RC = windres


DLL_CFLAGS = -O0 -g -std=c++11 ${WARNS} -Iinclude -D ADD_EXPORTS -fpermissive
DLL1_LDFLAGS = -shared -static-libgcc -static-libstdc++ -s -Wl,-Bstatic,--whole-archive -lwinpthread -Wl,--no-whole-archive -Wl,--subsystem,windows,--out-implib,lib/buildext.a
DLL2_LDFLAGS = -shared -static-libgcc -static-libstdc++ -s -Wl,-Bstatic,--whole-archive -lwinpthread -Wl,--no-whole-archive -Wl,--subsystem,windows,--out-implib,lib/new_upgrades.a
DLL3_LDFLAGS = -shared -static-libgcc -static-libstdc++ -s -Wl,-Bstatic,--whole-archive -lwinpthread -Wl,--no-whole-archive -Wl,--subsystem,windows,--out-implib,lib/buildexp.a
DLL4_LDFLAGS = -shared -static-libgcc -static-libstdc++ -s -Wl,-Bstatic,--whole-archive -lwinpthread -Wl,--no-whole-archive -Wl,--subsystem,windows,--out-implib,lib/elvenworker.a

.PHONY: all clean

all: bin/new_upgrades.asi bin/buildexp.asi bin/elvenworker.asi
clean:
	if exist bin\* del /q bin\*
	if exist lib\* del /q lib\*
	if exist obj\* del /q obj\*

bin lib obj:
	@if not exist "$@" mkdir "$@"

obj/buildings_expanded.o: src/buildings_expanded.cpp src/asi/sf_asi.h | obj
	${CC} ${DLL_CFLAGS} -c "$<" -o "$@"

obj/sf_asi.o: src/asi/sf_asi.cpp src/asi/sf_asi.h | obj
	${CC} ${DLL_CFLAGS} -c "$<" -o "$@"

obj/dllmain.o: src/dllmain.cpp src/asi/sf_asi.h | obj
	${CC} ${DLL1_LDFLAGS} -c "$<" -o "$@"

obj/new_upgrades.o: src/new_upgrades.cpp src/asi/sf_asi.h | obj
	${CC} -mgeneral-regs-only ${DLL_CFLAGS} -c "$<" -o "$@"

obj/elvenworker.o: src/elvenworker.cpp src/asi/sf_asi.h | obj
	${CC} -mgeneral-regs-only ${DLL_CFLAGS} -c "$<" -o "$@"


bin/elvenworker.asi: $(ELVEN_OBJ)| bin lib
	${CC} -o "$@" ${ELVEN_OBJ} ${DLL4_LDFLAGS}

bin/new_upgrades.asi: ${NEWUPGD_OBJ}| bin lib
	${CC} -o "$@" ${NEWUPGD_OBJ} ${DLL2_LDFLAGS}


bin/buildext.asi: ${BUILDEX_OBJS}| bin lib
	${CC} -o "$@" ${BUILDEX_OBJS} ${DLL1_LDFLAGS}

bin/buildexp.asi:  ${BUILDEXP_OBJS}| bin lib
	${CC} -o "$@" ${BUILDEXP_OBJS} ${DLL3_LDFLAGS}