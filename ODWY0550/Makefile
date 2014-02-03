# vbcc needs this environment variable so it can find its config file.
export VBCC:=../vbcc

VBCCZ = ../vbcc/bin/vc +z -c99 -O1 -DZ_MACHINE -DSAVE_AND_RESTORE -Iz-machine/include
MAXES = \$$MAX_ARRAYS=1100 \
        \$$MAX_ZCODE_SIZE=9000 \
        \$$MAX_STATIC_DATA=60000

all: adv550 adv550.z8

adv550: adv550.c advdat.c advdat.h
	gcc -std=c99 -O3 -pedantic -W -Wall -Wextra \
		-Wno-switch -Wno-unused-result \
		adv550.c advdat.c -o $@

adv550.z8: z-machine/Main.inf z-machine/stubs.inf z-machine/cstubs.s z-machine/adv550.s z-machine/advdat.s
	cd z-machine ; inform -v8 -ew~S~X~D $(MAXES) Main.inf ../adv550.z8

z-machine/adv550.s: adv550.c
	$(VBCCZ) -c $< -o $@ -module-name=adv550

z-machine/advdat.s: advdat.c
	$(VBCCZ) -c $< -o $@ -module-name=advdat

z-machine/cstubs.s: z-machine/cstubs.c
	$(VBCCZ) -c $< -o $@ -module-name=libc

clean:
	rm -f adv550.z8 z-machine/*.s
	rm -f adv550
