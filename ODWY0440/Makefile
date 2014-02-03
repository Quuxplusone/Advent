# vbcc needs this environment variable so it can find its config file.
export VBCC:=../vbcc

VBCCZ = ../vbcc/bin/vc +z -c99 -O1 -DZ_MACHINE -DSAVE_AND_RESTORE -Iz-machine/include
MAXES = \$$MAX_ARRAYS=1700 \
        \$$MAX_ZCODE_SIZE=68000 \
        \$$MAX_STATIC_DATA=88000

all: adv440 adv440.z8

adv440: adv440.c
	gcc -std=c99 -O3 -pedantic -W -Wall -Wextra \
		-Wno-switch -Wno-unused-result \
		adv440.c -o $@

adv440.z8: z-machine/Main.inf z-machine/stubs.inf z-machine/cstubs.s z-machine/adv440.s
	cd z-machine ; inform -v8 -ew~S~X~D $(MAXES) Main.inf ../adv440.z8

z-machine/adv440.s: adv440.c
	$(VBCCZ) -c $< -o $@ -module-name=adv440

z-machine/cstubs.s: z-machine/cstubs.c
	$(VBCCZ) -c $< -o $@ -module-name=libc

clean:
	rm -f adv440.z8 z-machine/*.s
	rm -f adv440
