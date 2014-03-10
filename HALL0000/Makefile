DATAFILES = advtext.h advent1.dat advent2.dat advent3.dat advent4.dat

all: advent $(DATAFILES)

advent: advent.o database.o english.o initial.o itverb.o score.o travel.o turn.o utility.o verb.o vocab.o
	gcc $^ -o $@

%.o: %.c
	gcc -c $^ -o $@

database.o: database.c $(DATAFILES)
	gcc -c database.c -o $@

$(DATAFILES): setup.c advent.h
	gcc setup.c -o setup
	./setup

clean:
	@rm -f *.o *.dat setup advent $(DATAFILES)
