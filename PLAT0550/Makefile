ifneq (, $(shell which f77))
FORTRAN:=f77
else ifneq (, $(shell which gfortran))
FORTRAN:=gfortran -std=legacy
else
$(error I couldn't find any way to compile Fortran on your system!)
endif

adv550: munge ADVENTURE.ACODE
	./munge
	$(FORTRAN) exec.f -o adv550

munge: munge.f munge.i
	$(FORTRAN) munge.f -o munge

clean:
	rm -f adv550 ADVINIT[1-4].DAT munge
