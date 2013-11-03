ifneq (, $(shell which f77))
FORTRAN:=f77
else ifneq (, $(shell which gfortran))
FORTRAN:=gfortran -std=legacy
else
$(error I couldn't find any way to compile Fortran on your system!)
endif

adv551: ADVTXT aamain.f asubs.f
	$(FORTRAN) aamain.f asubs.f -o adv551

asetup: asetup.f asubs.f
	$(FORTRAN) asetup.f asubs.f -o asetup

ADVTXT: asetup ADVDAT
	./asetup

clean:
	rm -f adv551 ADVTXT asetup
