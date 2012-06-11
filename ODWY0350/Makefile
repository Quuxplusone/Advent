# vbcc needs this environment variable so it can find its config file.
export VBCC:=../vbcc

VBCCZ = ../vbcc/bin/vc +z -c99 -O=3 -DZ_MACHINE -DSAVE_AND_RESTORE -Iz-machine/include
MAXES = \$$MAX_ARRAYS=800 \
        \$$MAX_ZCODE_SIZE=39000 \
        \$$MAX_STATIC_DATA=55000

all: advent advent.z8

advent: advent.c
	gcc -std=c99 -O3 -pedantic -W -Wall -Wextra \
		-Wno-switch -Wno-unused-result \
		advent.c -o $@

advent.z8: z-machine/Main.inf z-machine/stubs.inf z-machine/cstubs.s z-machine/advent.s
	cd z-machine ; inform -v8 -ew~S~X~D $(MAXES) Main.inf ../advent.z8

z-machine/advent.s: advent.c
	$(VBCCZ) -c $< -o $@ -module-name=advent

z-machine/cstubs.s: z-machine/cstubs.c
	$(VBCCZ) -c $< -o $@ -module-name=libc

clean:
	rm -f advent.z8 z-machine/*.s
	rm -f advent
