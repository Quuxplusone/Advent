
# used to create vbcc, vc and ucpp
CC = gcc -std=c9x -g -DHAVE_AOS4 #-DHAVE_ECPP -DHAVE_MISRA
LDFLAGS = -lm

# native version; used to create dtgen
NCC = $(CC)
NLDFLAGS = $(LDFLAGS)

all: bin/vc bin/vprof bin/vbcc$(TARGET) #bin/vcpp

vbccs: bin/vbccs$(TARGET)

bin/vc: frontend/vc.c
	$(CC) frontend/vc.c -o bin/vc $(LDFLAGS)

bin/vprof: vprof/vprof.c
	$(CC) vprof/vprof.c -o bin/vprof $(LDFLAGS)

doc/vbcc.pdf:
	texi2dvi --pdf doc/vbcc.texi

doc/vbcc.html:
	(cd doc;texi2html -split=chapter -nosec_nav -frames vbcc.texi)
	sed -e s/vbcc_13/vbcc_1/ <doc/vbcc_frame.html >doc/vbcc.html

vcppobjs = vcpp/cpp.o vcpp/eval.o vcpp/getopt.o vcpp/hideset.o vcpp/include.o \
	   vcpp/lex.o vcpp/macro.o vcpp/nlist.o vcpp/tokens.o vcpp/unix.o

vbcc.tar.gz:
	(cd ..;tar zcvf vbcc.tar.gz vbcc/Makefile vbcc/*.[ch] vbcc/datatypes/*.[ch] vbcc/doc/*.texi vbcc/frontend/vc.c vbcc/machines/*/machine.[ch] vbcc/machines/*/machine.dt vbcc/machines/*/schedule.[ch] vbcc/ucpp/*.[ch] vbcc/ucpp/README vbcc/vprof/vprof.c vbcc/vsc/vsc.[ch])

bin/osekrm: osekrm.c
	$(CC) osekrm.c -o bin/osekrm

dist: bin/osekrm
	mv supp.h t1
	mv supp.c t2
	mv main.c t3
	mv machines/ppc/machine.c t4
	mv declaration.c t5
	mv flow.c t6
	mv ic.c t7
	mv parse_expr.c t8
	mv statements.c t9
	mv rd.c t10
	mv type_expr.c t11
	bin/osekrm <t1 >supp.h
	bin/osekrm <t2 >supp.c
	bin/osekrm <t3 >main.c
	bin/osekrm <t4 >machines/ppc/machine.c
	bin/osekrm <t5 >declaration.c
	bin/osekrm <t6 >flow.c
	bin/osekrm <t7 >ic.c
	bin/osekrm <t8 >parse_expr.c
	bin/osekrm <t9 >statements.c
	bin/osekrm <t10 >rd.c
	bin/osekrm <t11 >type_expr.c
	make vbcc.tar.gz
	mv t1 supp.h
	mv t2 supp.c
	mv t3 main.c
	mv t4 machines/ppc/machine.c
	mv t5 declaration.c
	mv t6 flow.c
	mv t7 ic.c
	mv t8 parse_expr.c
	mv t9 statements.c
	mv t10 rd.c
	mv t11 type_expr.c	

bin/vcpp: $(vcppobjs)
	$(CC) $(LDFLAGS) $(vcppobjs) -o bin/vcpp

vcpp/cpp.o: vcpp/cpp.c vcpp/cpp.h
	$(CC) -c -Ivcpp vcpp/cpp.c -o vcpp/cpp.o

vcpp/eval.o: vcpp/eval.c vcpp/cpp.h
	$(CC) -c -Ivcpp vcpp/eval.c -o vcpp/eval.o

vcpp/getopt.o: vcpp/getopt.c
	$(CC) -c -Ivcpp vcpp/getopt.c -o vcpp/getopt.o

vcpp/hideset.o: vcpp/hideset.c vcpp/cpp.h
	$(CC) -c -Ivcpp vcpp/hideset.c -o vcpp/hideset.o

vcpp/include.o: vcpp/include.c vcpp/cpp.h
	$(CC) -c -Ivcpp vcpp/include.c -o vcpp/include.o

vcpp/lex.o: vcpp/lex.c vcpp/cpp.h
	$(CC) -c -Ivcpp vcpp/lex.c -o vcpp/lex.o

vcpp/macro.o: vcpp/macro.c vcpp/cpp.h
	$(CC) -c -Ivcpp vcpp/macro.c -o vcpp/macro.o

vcpp/nlist.o: vcpp/nlist.c vcpp/cpp.h
	$(CC) -c -Ivcpp vcpp/nlist.c -o vcpp/nlist.o

vcpp/tokens.o: vcpp/tokens.c vcpp/cpp.h
	$(CC) -c -Ivcpp vcpp/tokens.c -o vcpp/tokens.o

vcpp/unix.o: vcpp/unix.c vcpp/cpp.h
	$(CC) -c -Ivcpp vcpp/unix.c -o vcpp/unix.o

TRGDIR = machines/$(TARGET)

bobjects = $(TRGDIR)/main.o $(TRGDIR)/vars.o $(TRGDIR)/declaration.o \
	   $(TRGDIR)/parse_expr.o $(TRGDIR)/type_expr.o $(TRGDIR)/ic.o \
	   $(TRGDIR)/machine.o $(TRGDIR)/statements.o \
	   $(TRGDIR)/supp.o $(TRGDIR)/dt.o \
           $(TRGDIR)/assert.o $(TRGDIR)/cpp.o $(TRGDIR)/hash.o \
           $(TRGDIR)/lexer.o $(TRGDIR)/macro.o $(TRGDIR)/mem.o \
           $(TRGDIR)/eval.o
#           $(TRGDIR)/AdjList.o $(TRGDIR)/DUChain.o \
#           $(TRGDIR)/ICodeInsertion.o $(TRGDIR)/NodeList.o \
#           $(TRGDIR)/RAllocMain.o $(TRGDIR)/Web.o

fobjects = $(TRGDIR)/opt.o $(TRGDIR)/av.o $(TRGDIR)/rd.o $(TRGDIR)/regs.o \
	   $(TRGDIR)/flow.o $(TRGDIR)/cse.o $(TRGDIR)/cp.o $(TRGDIR)/loop.o \
	   $(TRGDIR)/alias.o $(bobjects)

sobjects = $(TRGDIR)/opts.o $(TRGDIR)/regss.o $(bobjects)

tasm	 = $(TRGDIR)/supp.o $(TRGDIR)/tasm.o $(TRGDIR)/dt.o \
	   $(TRGDIR)/opt.o $(TRGDIR)/av.o $(TRGDIR)/rd.o $(TRGDIR)/regs.o \
	   $(TRGDIR)/flow.o $(TRGDIR)/cse.o $(TRGDIR)/cp.o $(TRGDIR)/loop.o \
	   $(TRGDIR)/alias.o $(TRGDIR)/machine.o

mbasic	 = $(TRGDIR)/supp.o $(TRGDIR)/mbasic.o $(TRGDIR)/dt.o \
	   $(TRGDIR)/opt.o $(TRGDIR)/av.o $(TRGDIR)/rd.o $(TRGDIR)/regs.o \
	   $(TRGDIR)/flow.o $(TRGDIR)/cse.o $(TRGDIR)/cp.o $(TRGDIR)/loop.o \
	   $(TRGDIR)/alias.o $(TRGDIR)/machine.o

minicomp	 = $(TRGDIR)/supp.o $(TRGDIR)/minicompg.tab.o $(TRGDIR)/minicomp.o $(TRGDIR)/dt.o \
	   $(TRGDIR)/opt.o $(TRGDIR)/av.o $(TRGDIR)/rd.o $(TRGDIR)/regs.o \
	   $(TRGDIR)/flow.o $(TRGDIR)/cse.o $(TRGDIR)/cp.o $(TRGDIR)/loop.o \
	   $(TRGDIR)/alias.o $(TRGDIR)/machine.o

vscobjects = $(TRGDIR)/vsc.o $(TRGDIR)/schedule.o

bin/vbcc$(TARGET): $(fobjects)
	$(CC) $(LDFLAGS) $(fobjects) -o bin/vbcc$(TARGET)

bin/vbccs$(TARGET): $(sobjects)
	$(CC) $(LDFLAGS) $(sobjects) -o bin/vbccs$(TARGET)

bin/vsc$(TARGET): $(vscobjects)
	$(CC) $(LDFLAGS) $(vscobjects) -o bin/vsc$(TARGET)

bin/tasm$(TARGET): $(tasm)
	$(CC) $(LDFLAGS) $(tasm) -o bin/tasm$(TARGET)

bin/mbasic$(TARGET): $(mbasic)
	$(CC) $(LDFLAGS) $(mbasic) -o bin/mbasic$(TARGET)

bin/minicomp$(TARGET): $(minicomp)
	$(CC) $(LDFLAGS) $(minicomp) -o bin/minicomp$(TARGET)

bin/dtgen: datatypes/dtgen.c datatypes/datatypes.h datatypes/dtconv.h
	$(NCC) datatypes/dtgen.c -o bin/dtgen -Idatatypes $(NLDFLAGS)

$(TRGDIR)/dt.h: bin/dtgen $(TRGDIR)/machine.dt
	bin/dtgen $(TRGDIR)/machine.dt $(TRGDIR)/dt.h $(TRGDIR)/dt.c

$(TRGDIR)/dt.c: bin/dtgen $(TRGDIR)/machine.dt
	bin/dtgen $(TRGDIR)/machine.dt $(TRGDIR)/dt.h $(TRGDIR)/dt.c

$(TRGDIR)/dt.o: $(TRGDIR)/dt.h $(TRGDIR)/dt.c
	$(CC) -c $(TRGDIR)/dt.c -o $(TRGDIR)/dt.o -I$(TRGDIR) -Idatatypes

$(TRGDIR)/tasm.o: tasm.c supp.h $(TRGDIR)/machine.h $(TRGDIR)/dt.h
	$(CC) -c tasm.c -o $(TRGDIR)/tasm.o -I$(TRGDIR)

$(TRGDIR)/mbasic.o: mbasic.c supp.h $(TRGDIR)/machine.h $(TRGDIR)/dt.h
	$(CC) -c mbasic.c -o $(TRGDIR)/mbasic.o -I$(TRGDIR)

$(TRGDIR)/minicomp.o: minicomp.c minicomp.h supp.h $(TRGDIR)/machine.h $(TRGDIR)/dt.h
	$(CC) -c minicomp.c -o $(TRGDIR)/minicomp.o -I$(TRGDIR)

$(TRGDIR)/minicompg.tab.o: minicompg.y minicomplexer.c minicomp.h supp.h
	bison minicompg.y
	$(CC) -c minicompg.tab.c -o $(TRGDIR)/minicompg.tab.o -I$(TRGDIR)

$(TRGDIR)/supp.o: supp.c supp.h $(TRGDIR)/machine.h $(TRGDIR)/dt.h
	$(CC) -c supp.c -o $(TRGDIR)/supp.o -I$(TRGDIR)

$(TRGDIR)/main.o: main.c vbc.h supp.h vbcc_cpp.h ucpp/cpp.h $(TRGDIR)/machine.h $(TRGDIR)/dt.h
	$(CC) -c main.c -o $(TRGDIR)/main.o -I$(TRGDIR)

$(TRGDIR)/vars.o: vars.c vbc.h supp.h $(TRGDIR)/machine.h errors.h $(TRGDIR)/dt.h
	$(CC) -c vars.c -o $(TRGDIR)/vars.o -I$(TRGDIR)

$(TRGDIR)/declaration.o: declaration.c vbc.h supp.h vbcc_cpp.h ucpp/cpp.h $(TRGDIR)/machine.h $(TRGDIR)/dt.h
	$(CC) -c declaration.c -o $(TRGDIR)/declaration.o -I$(TRGDIR)

$(TRGDIR)/parse_expr.o: parse_expr.c vbc.h supp.h vbcc_cpp.h ucpp/cpp.h $(TRGDIR)/machine.h $(TRGDIR)/dt.h
	$(CC) -c parse_expr.c -o $(TRGDIR)/parse_expr.o -I$(TRGDIR)

$(TRGDIR)/type_expr.o: type_expr.c vbc.h supp.h $(TRGDIR)/machine.h $(TRGDIR)/dt.h
	$(CC) -c type_expr.c -o $(TRGDIR)/type_expr.o -I$(TRGDIR)

$(TRGDIR)/ic.o: ic.c vbc.h supp.h $(TRGDIR)/machine.h $(TRGDIR)/dt.h
	$(CC) -c ic.c -o $(TRGDIR)/ic.o -I$(TRGDIR)

$(TRGDIR)/statements.o: statements.c vbc.h supp.h vbcc_cpp.h ucpp/cpp.h $(TRGDIR)/machine.h $(TRGDIR)/dt.h
	$(CC) -c statements.c -o $(TRGDIR)/statements.o -I$(TRGDIR)

$(TRGDIR)/opt.o: opt.c opt.h supp.h $(TRGDIR)/machine.h $(TRGDIR)/dt.h
	$(CC) -c opt.c -o $(TRGDIR)/opt.o -I$(TRGDIR)

$(TRGDIR)/av.o: av.c opt.h supp.h $(TRGDIR)/machine.h $(TRGDIR)/dt.h
	$(CC) -c av.c -o $(TRGDIR)/av.o -I$(TRGDIR)

$(TRGDIR)/rd.o: rd.c opt.h supp.h $(TRGDIR)/machine.h $(TRGDIR)/dt.h
	$(CC) -c rd.c -o $(TRGDIR)/rd.o -I$(TRGDIR)

$(TRGDIR)/regs.o: regs.c opt.h supp.h $(TRGDIR)/machine.h $(TRGDIR)/dt.h
	$(CC) -c regs.c -o $(TRGDIR)/regs.o -I$(TRGDIR)

$(TRGDIR)/flow.o: flow.c opt.h supp.h $(TRGDIR)/machine.h $(TRGDIR)/dt.h
	$(CC) -c flow.c -o $(TRGDIR)/flow.o -I$(TRGDIR)

$(TRGDIR)/cse.o: cse.c opt.h supp.h $(TRGDIR)/machine.h $(TRGDIR)/dt.h
	$(CC) -c cse.c -o $(TRGDIR)/cse.o -I$(TRGDIR)

$(TRGDIR)/cp.o: cp.c opt.h supp.h $(TRGDIR)/machine.h $(TRGDIR)/dt.h
	$(CC) -c cp.c -o $(TRGDIR)/cp.o -I$(TRGDIR)

$(TRGDIR)/loop.o: loop.c opt.h supp.h $(TRGDIR)/machine.h $(TRGDIR)/dt.h
	$(CC) -c loop.c -o $(TRGDIR)/loop.o -I$(TRGDIR)

$(TRGDIR)/alias.o: alias.c opt.h supp.h $(TRGDIR)/machine.h $(TRGDIR)/dt.h
	$(CC) -c alias.c -o $(TRGDIR)/alias.o -I$(TRGDIR)

$(TRGDIR)/preproc.o: preproc.c vbpp.h supp.h vbc.h $(TRGDIR)/dt.h
	$(CC) -c preproc.c -o $(TRGDIR)/preproc.o -I$(TRGDIR)

$(TRGDIR)/assert.o: ucpp/assert.c ucpp/cpp.h ucpp/mem.h ucpp/hash.h ucpp/tune.h $(TRGDIR)/dt.h
	$(CC) -DNO_UCPP_ERROR_FUNCTIONS -c ucpp/assert.c -o $(TRGDIR)/assert.o -I$(TRGDIR)

$(TRGDIR)/cpp.o: ucpp/cpp.c ucpp/cpp.h ucpp/mem.h ucpp/hash.h ucpp/tune.h $(TRGDIR)/dt.h
	$(CC) -DNO_UCPP_ERROR_FUNCTIONS -c ucpp/cpp.c -o $(TRGDIR)/cpp.o -I$(TRGDIR)

$(TRGDIR)/hash.o: ucpp/hash.c ucpp/cpp.h ucpp/mem.h ucpp/hash.h ucpp/tune.h $(TRGDIR)/dt.h
	$(CC) -DNO_UCPP_ERROR_FUNCTIONS -c ucpp/hash.c -o $(TRGDIR)/hash.o -I$(TRGDIR)

$(TRGDIR)/lexer.o: ucpp/lexer.c ucpp/cpp.h ucpp/mem.h ucpp/hash.h ucpp/tune.h $(TRGDIR)/dt.h
	$(CC) -DNO_UCPP_ERROR_FUNCTIONS -c ucpp/lexer.c -o $(TRGDIR)/lexer.o -I$(TRGDIR)

$(TRGDIR)/macro.o: ucpp/macro.c ucpp/cpp.h ucpp/mem.h ucpp/hash.h ucpp/tune.h $(TRGDIR)/dt.h
	$(CC) -DNO_UCPP_ERROR_FUNCTIONS -c ucpp/macro.c -o $(TRGDIR)/macro.o -I$(TRGDIR)

$(TRGDIR)/mem.o: ucpp/mem.c ucpp/cpp.h ucpp/mem.h ucpp/hash.h ucpp/tune.h $(TRGDIR)/dt.h
	$(CC) -DNO_UCPP_ERROR_FUNCTIONS -c ucpp/mem.c -o $(TRGDIR)/mem.o -I$(TRGDIR)

$(TRGDIR)/eval.o: ucpp/eval.c ucpp/cpp.h ucpp/mem.h ucpp/tune.h $(TRGDIR)/dt.h
	$(CC) -DNO_UCPP_ERROR_FUNCTIONS -c ucpp/eval.c -o $(TRGDIR)/eval.o -I$(TRGDIR)

$(TRGDIR)/machine.o: $(TRGDIR)/machine.c supp.h $(TRGDIR)/machine.h $(TRGDIR)/dt.h dwarf2.c
	$(CC) -c $(TRGDIR)/machine.c -o $(TRGDIR)/machine.o -I$(TRGDIR) -I.

$(TRGDIR)/opts.o: opt.c opt.h supp.h $(TRGDIR)/machine.h $(TRGDIR)/dt.h
	$(CC) -c -DNO_OPTIMIZER opt.c -o $(TRGDIR)/opts.o -I$(TRGDIR)

$(TRGDIR)/regss.o: regs.c opt.h supp.h $(TRGDIR)/machine.h $(TRGDIR)/dt.h
	$(CC) -c -DNO_OPTIMIZER regs.c -o $(TRGDIR)/regss.o -I$(TRGDIR)

$(TRGDIR)/vsc.o: vsc/vsc.h vsc/vsc.c $(TRGDIR)/schedule.h
	$(CC) -c vsc/vsc.c -o $(TRGDIR)/vsc.o -I$(TRGDIR)

$(TRGDIR)/schedule.o: vsc/vsc.h $(TRGDIR)/schedule.h $(TRGDIR)/schedule.c
	$(CC) -c $(TRGDIR)/schedule.c -o $(TRGDIR)/schedule.o -I$(TRGDIR) -Ivsc


# Graph coloring register allocator by Alex
$(TRGDIR)/AdjList.o: GCRegAlloc/AdjList.c opt.h supp.h $(TRGDIR)/machine.h $(TRGDIR)/dt.h
	$(CC) -c GCRegAlloc/AdjList.c -o $(TRGDIR)/AdjList.o -IGCRegAlloc -I$(TRGDIR)

$(TRGDIR)/DUChain.o: GCRegAlloc/DUChain.c opt.h supp.h $(TRGDIR)/machine.h $(TRGDIR)/dt.h
	$(CC) -c GCRegAlloc/DUChain.c -o $(TRGDIR)/DUChain.o -IGCRegAlloc -I$(TRGDIR)

$(TRGDIR)/ICodeInsertion.o: GCRegAlloc/ICodeInsertion.c opt.h supp.h $(TRGDIR)/machine.h $(TRGDIR)/dt.h
	$(CC) -c GCRegAlloc/ICodeInsertion.c -o $(TRGDIR)/ICodeInsertion.o -IGCRegAlloc -I$(TRGDIR)

$(TRGDIR)/NodeList.o: GCRegAlloc/NodeList.c opt.h supp.h $(TRGDIR)/machine.h $(TRGDIR)/dt.h
	$(CC) -c GCRegAlloc/NodeList.c -o $(TRGDIR)/NodeList.o -IGCRegAlloc -I$(TRGDIR)

$(TRGDIR)/RAllocMain.o: GCRegAlloc/RAllocMain.c opt.h supp.h $(TRGDIR)/machine.h $(TRGDIR)/dt.h
	$(CC) -c GCRegAlloc/RAllocMain.c -o $(TRGDIR)/RAllocMain.o -IGCRegAlloc -I$(TRGDIR)

$(TRGDIR)/Web.o: GCRegAlloc/Web.c opt.h supp.h $(TRGDIR)/machine.h $(TRGDIR)/dt.h
	$(CC) -c GCRegAlloc/Web.c -o $(TRGDIR)/Web.o -IGCRegAlloc -I$(TRGDIR)
