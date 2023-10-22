# vbcc needs this environment variable so it can find its config file.
export VBCC:=../vbcc

VBCCZ = ../vbcc/bin/vc +z -c99 -O1 -DZ_MACHINE -DSAVE_AND_RESTORE -Iz-machine/include
MAXES = \$$MAX_ARRAYS=700 \
        \$$MAX_ZCODE_SIZE=38000 \
        \$$MAX_STATIC_DATA=48000

all: advent3 advent3.z8

advent3: advent3.c
	gcc -std=c99 -O3 -pedantic -W -Wall -Wextra \
		-Wno-switch -Wno-unused-result \
		advent3.c -o $@

advent3.z8: z-machine/Main.inf z-machine/stubs.inf z-machine/cstubs.s z-machine/advent3.s
	cd z-machine ; inform -v8 -ew~S~X~D $(MAXES) Main.inf ../advent3.z8

z-machine/advent3.s: advent3.c
	$(VBCCZ) -c $< -o $@ -module-name=advent3

z-machine/cstubs.s: z-machine/cstubs.c
	$(VBCCZ) -c $< -o $@ -module-name=libc

clean:
	rm -f advent3.z8 z-machine/*.s
	rm -f advent3
