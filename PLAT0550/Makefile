
adv550: munge ADVENTURE.ACODE
	./munge
	f77 exec.f -o adv550

munge: munge.f munge.i
	f77 munge.f -o munge

clean:
	rm -f adv550 ADVINIT[1-4].DAT munge
