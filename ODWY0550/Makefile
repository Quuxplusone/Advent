
all: adv550

adv550: adv00.c adv01.c adv02.c adv03.c adv04.c adv05.c
	gcc -DCONSOLE -pedantic -W -Wall -Wextra \
		$^ -o $@

clean:
	rm -f adv550
