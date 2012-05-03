# vbcc needs this environment variable so it can find its config file.
export VBCC:=../vbcc

VBCCZ = ../vbcc/bin/vc +z -O=3 -DZ_MACHINE -Iz-machine/include
MAXES = \$$MAX_ARRAYS=800 \
        \$$MAX_ZCODE_SIZE=42000 \
		\$$MAX_STATIC_DATA=57000

all: advent advent.z8

advent: advent.c
	gcc -O3 -ansi -pedantic -W -Wall -Wextra \
		-Wno-switch -Wno-overlength-strings -Wno-unused-result \
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
