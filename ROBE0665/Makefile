# Makefile for newadv directory
#****************************************************************

# ***************************************************************
# Parameters to control Makefile operation

CC = gcc
CFLAGS = -g
F77 = /usr/bin/gfortran -g -std=legacy -ffixed-line-length-none


# ***************************************************************
# Entry to bring the package up to date
#    The "make all" entry should be the first real entry

all: newadv

setup: setup.o advlib.o common.o csub.o
	$(F77) -o setup setup.o advlib.o common.o csub.o

newadv: newadv.o advlib.o aparse.o data.o csub.o
	$(F77) -Bstatic -o newadv newadv.o advlib.o aparse.o data.o csub.o

setup.o: setup.F \
		adjcom.h allcom.h bitcom.h bltcom.h concom.h detcom.h \
		diecom.h dwfcom.h gencom.h hldcom.h hntcom.h lincom.h \
		liqcom.h loccom.h ltxcom.h milcom.h mnecom.h objcom.h \
		params.h placom.h prpcom.h trvcom.h txtcom.h usrcom.h \
		utxcom.h vercom.h voccom.h wizcom.h wrdcom.h
	$(F77) -c setup.F

newadv.o: newadv.F \
		adjcom.h allcom.h bitcom.h bltcom.h concom.h detcom.h \
		diecom.h dwfcom.h gencom.h hldcom.h hntcom.h lincom.h \
		liqcom.h loccom.h ltxcom.h milcom.h mnecom.h objcom.h \
		params.h placom.h prpcom.h trvcom.h txtcom.h usrcom.h \
		utxcom.h vercom.h voccom.h wizcom.h wrdcom.h
	$(F77) -c newadv.F

advlib.o: advlib.F \
		bitcom.h bltcom.h concom.h detcom.h diecom.h dwfcom.h \
		hldcom.h hntcom.h lincom.h liqcom.h loccom.h mnecom.h \
		objcom.h params.h placom.h txtcom.h utxcom.h voccom.h \
		wrdcom.h
	$(F77) -c advlib.F

aparse.o: aparse.F \
		adjcom.h allcom.h bltcom.h diecom.h dwfcom.h gencom.h \
		hntcom.h lincom.h liqcom.h loccom.h ltxcom.h mnecom.h \
		objcom.h params.h placom.h prpcom.h trvcom.h utxcom.h \
		wrdcom.h
	$(F77) -c aparse.F

allcom.o: allcom.F
	$(F77) -c allcom.F

allcom.F: adjcom.h allcom.h bitcom.h bltcom.h concom.h detcom.h diecom.h \
		dwfcom.h gencom.h hldcom.h hntcom.h lincom.h liqcom.h \
		loccom.h ltxcom.h milcom.h mnecom.h objcom.h params.h \
		placom.h prpcom.h trvcom.h txtcom.h usrcom.h utxcom.h \
		vercom.h voccom.h wizcom.h wrdcom.h Makefile
	echo '#include "params.h"' > allcom.F
	echo '	IMPLICIT INTEGER (A-Z)' >> allcom.F
	ls -1 [a-z]*com.h | sed 's/.*/#include "&"/' >> allcom.F
	echo '	END' >> allcom.F

data.o: data.c
	gcc -c data.c

data.c: setup text.dat
	setup

common.h: allcom.o nmcom.awk
	nm allcom.o | awk -f nmcom.awk > common.h

volatile.h: common.h volatile.fg
	-fgrep -f volatile.fg common.h > volatile.h

common.o: common.c common.h
	$(CC) $(CFLAGS) -c common.c

csub.o: csub.c common.h volatile.h
	$(CC) $(CFLAGS) -c csub.c

.F.o:
	$(F77) -c $*.F


# ***************************************************************
# Standard entries to remove files from the directories
#    tidy  -- eliminate unwanted files
#    clean -- delete derived files in preparation for rebuild

tidy:
	rm -f ,* .,* core a.out *~

clean: tidy
	rm -f *.o *.a newadv
