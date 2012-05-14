
all: advent advent.pdf

advent: advent.c
	gcc advent.c -o $@

advent.c: advent.w
	ctangle advent.w

advent.pdf: advent.w
	-cweave -bhp advent.w
	-pdftex -interaction=batchmode advent.tex
	rm -f advent.idx advent.log advent.scn advent.toc
	rm -f advent.tex

clean:
	rm -f advent advent.c
