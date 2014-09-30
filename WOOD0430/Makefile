# NOTE: This version of Adventure was developed by the author on private
# equipment, and has been ported to Sun for entertainment purposes only.
# The author (Don Woods) retains full rights to the work.

DBX= -w
OBJS=main.o init.o actions1.o actions2.o score.o misc.o datime.o

.c.o:
	gcc -O $(DBX) -c $<

adv430:	$(OBJS)
	gcc -O $(DBX) -o adv430 $(OBJS)

main.o:		misc.h funcs.h

init.o:		misc.h main.h share.h funcs.h

actions1.o:	misc.h main.h share.h funcs.h

actions2.o:	misc.h main.h share.h funcs.h

score.o:	misc.h main.h share.h

misc.o:		misc.h main.h

clean:
	rm -rf adv430 $(OBJS)
	rm -rf adventure.data
