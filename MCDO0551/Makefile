
adv551: ADVTXT aamain.f asubs.f
	f77 aamain.f asubs.f -o adv551

asetup: asetup.f asubs.f
	f77 asetup.f asubs.f -o asetup

ADVTXT: asetup ADVDAT
	./asetup

clean:
	rm -f adv551 ADVTXT asetup
