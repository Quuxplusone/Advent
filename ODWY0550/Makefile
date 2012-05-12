
all: adv550

adv550: adv00.c adv02.c adv03.c adv04.c adv05.c
	gcc -g -std=c99 -pedantic -W -Wall -Wextra -funsigned-char $^ -o $@

clean:
	rm -f adv550
