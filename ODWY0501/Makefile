# vbcc needs this environment variable so it can find its config file.
export VBCC:=../vbcc

VBCCZ = ../vbcc/bin/vc +z -c99 -O=3 -DZ_MACHINE -DSAVE_AND_RESTORE -Iz-machine/include
MAXES = \$$MAX_ARRAYS=1700 \
        \$$MAX_ZCODE_SIZE=59000 \
        \$$MAX_STATIC_DATA=88000

all: adv551

adv551: adv551.o getwds.o objprop.o
	gcc -std=c99 -O3 -pedantic -W -Wall -Wextra \
		-Wno-switch -Wno-unused-result \
		$^ -o $@

%.o: %.c
	gcc -std=c99 -O3 -pedantic -W -Wall -Wextra \
		-Wno-switch -Wno-unused-result \
		$^ -c -o $@

adv551.z8: z-machine/Main.inf z-machine/stubs.inf z-machine/cstubs.s z-machine/advent.s
	cd z-machine ; inform -v8 -ew~S~X~D $(MAXES) Main.inf ../adv551.z8

z-machine/advent.s: adv551.c
	$(VBCCZ) -c $< -o $@ -module-name=advent

z-machine/cstubs.s: z-machine/cstubs.c
	$(VBCCZ) -c $< -o $@ -module-name=libc

clean:
	rm -f adv551.z8 z-machine/*.s
	rm -f adv551 *.o
