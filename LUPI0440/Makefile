ifneq (, $(shell which f77))
FORTRAN:=f77
else ifneq (, $(shell which gfortran))
FORTRAN:=gfortran
else
$(error I couldn't find any way to compile Fortran on your system!)
endif

adv440: advII.dat advII.f
	$(FORTRAN) advII.f -o adv440

clean:
	rm -f adv440
