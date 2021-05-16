# vbcc needs this environment variable so it can find its config file.
export VBCC:=../vbcc

VBCCZ = ../vbcc/bin/vc +z -c99 -O1 -DZ_MACHINE -DSAVE_AND_RESTORE -Iz-machine/include
MAXES = \$$MAX_ARRAYS=700 \
        \$$MAX_ZCODE_SIZE=38000 \
        \$$MAX_STATIC_DATA=48000

all: almazar almazar.z8

almazar: almazar.c
	gcc -std=c99 -O3 -pedantic -W -Wall -Wextra \
		-Wno-switch -Wno-unused-result \
		almazar.c -o $@

almazar.z8: z-machine/Main.inf z-machine/stubs.inf z-machine/cstubs.s z-machine/almazar.s
	cd z-machine ; inform -v8 -ew~S~X~D $(MAXES) Main.inf ../almazar.z8

z-machine/almazar.s: almazar.c
	$(VBCCZ) -c $< -o $@ -module-name=almazar

z-machine/cstubs.s: z-machine/cstubs.c
	$(VBCCZ) -c $< -o $@ -module-name=libc

clean:
	rm -f almazar.z8 z-machine/*.s
	rm -f almazar
