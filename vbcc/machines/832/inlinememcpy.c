/* FIXME - emit a memcpy function with weak linkage if -size is specified */

void emit_inlinememcpy(FILE *f,struct IC *p, int t)
{
	int srctype=t;
	int srcr = t1;
	int dstr=0;
	int cntr=0;
	int savec=1;
	int saved=0;
	int wordcopy;
	int bytecopy;
	zmax copysize = opsize(p);
	int unrollwords=0;
	int unrollbytes=0;

	// Can we create larger code in the interests of speed?  If so, partially unroll the copy.
	wordcopy = copysize & ~3;
	bytecopy = copysize - wordcopy;

	if (wordcopy < 32 && !optsize)
		unrollwords=1;
	if (bytecopy < 5)
		unrollbytes=1;

	cleartempobj(f,t1);
	cleartempobj(f,tmp);

	// Even if a register is available we still have to save it because the current function wouldn't
	// but the parent function may be using it.  Therefore we might as well use a hardcoded register.
	dstr=t1+1;
	if(p->z.flags&(REG|DREFOBJ)==REG)
		dstr=p->z.reg;
	// FIXME - check this logic.  If the target is a register, it may not be a scratch register.
	if(regs[dstr]) // Scratch register - only need to save if it's in use?
	{
		saved=1;
		emit(f,"\tmt\t%s\n",regnames[dstr]);
		emit(f,"\tstdec\t%s\n",regnames[sp]);
		pushed+=4;
	}

	cntr=t1+2;
	if(cntr==dstr)	/* Use r1 instead of r2 if the dest pointer is in r2 already. */
		cntr=t1+1;
	if(cntr==srcr)  /* If cntr && srcr now clash, use r3 instead */
		cntr=t1+3;

	if((unrollwords && unrollbytes) || regs[cntr]==0)
		savec=0;
	else {
		emit(f,"\tmt\t%s\n",regnames[cntr]);
		emit(f,"\tstdec\t%s\n",regnames[sp]);
		pushed+=4;
	}

	/* Prepare source register */
	if ((t & NQ) == CHAR && (opsize(p) != 1)) {
		emit(f, "\t\t\t\t\t// (char with size!=1 -> array of unknown type)\n");
		srctype = ARRAY;	// FIXME - ugly hack
	}
	emit_objtoreg(f, &p->q1, srctype,srcr);

	/* Prepare destination register */

	emit_prepobj(f, &p->z, t, dstr, 0);

	if (p->z.flags & REG) {
		if(p->z.reg!=dstr) {// Move target register to dstr
			emit(f, "\tmt\t%s\n", regnames[p->z.reg]);
			emit(f, "\tmr\t%s\n", regnames[dstr]);
		}
	}

	emit(f, "\t\t\t\t\t// Copying %d words and %d bytes to %s\n", wordcopy / 4, bytecopy,
	     p->z.v ? p->z.v->identifier : "(null)");
//	printf("memcpy: Copying %d words and %d bytes to %s\n", wordcopy / 4, bytecopy,
//	     p->z.v ? p->z.v->identifier : "(null)");

//	if(!p->z.v)
//		printf("No z->v: z flags: %x\n",p->z.flags);

	// Prepare the copy
	// FIXME - we don't necessarily have a valid z->v!  If not, where does the target come from?
	// Stack based variable?

	if (unrollwords) {
		wordcopy >>= 2;
		if (wordcopy) {
			emit(f, "\t\t\t\t\t// Copying %d words to %s\n", wordcopy, p->z.v ? p->z.v->identifier : "(null)");
		}
		while (wordcopy--) {
			emit(f, "\tldinc\t%s\n\tstinc\t%s\n", regnames[srcr], regnames[dstr]);
		}
	} else if(wordcopy) {
		emit(f, "\t\t\t\t\t// Copying %d words to %s\n", wordcopy / 4, p->z.v ? p->z.v->identifier : "(null)");
		// Copy bytes...
		emit_constanttotemp(f, wordcopy);
		emit(f, "\taddt\t%s\n", regnames[dstr]);
		emit(f, "\tmr\t%s\n", regnames[cntr]);
		emit(f, ".cpy%swordloop%d:\n", p->z.v ? p->z.v->identifier : "null", loopid);
		emit(f, "\tldinc\t%s\n\tstinc\t%s\n", regnames[srcr], regnames[dstr]);
		emit(f, "\tmt\t%s\n\tcmp\t%s\n", regnames[dstr], regnames[cntr]);
		emit(f, "\tcond\tNEQ\n");
		emit(f, "\t\t.lipcrel\t.cpy%swordloop%d\n", p->z.v ? p->z.v->identifier : "null", loopid);
		emit(f, "\t\tadd\t%s\n", regnames[pc]);
	}

	if (unrollbytes) {
		if (bytecopy)
			emit(f, "\t\t\t\t\t// Copying %d byte tail to %s\n", bytecopy,p->z.v ? p->z.v->identifier : "null");
		while (bytecopy--)
			emit(f, "\tldbinc\t%s\n\tstbinc\t%s\n", regnames[srcr], regnames[dstr]);
	} else if (bytecopy) {
		emit(f, "\t\t\t\t\t// Copying %d bytes to %s\n", bytecopy, p->z.v ? p->z.v->identifier : "null");
		// Copy bytes...
		emit_constanttotemp(f, bytecopy);
		emit(f, "\taddt\t%s\n", regnames[dstr]);
		emit(f, "\tmr\t%s\n", regnames[cntr]);
		emit(f, ".cpy%sloop%d:\n", p->z.v ? p->z.v->identifier : "null", loopid);
		emit(f, "\tldbinc\t%s\n\tstbinc\t%s\n", regnames[srcr], regnames[dstr]);
		emit(f, "\tmt\t%s\n\tcmp\t%s\n", regnames[dstr], regnames[cntr]);
		emit(f, "\tcond\tNEQ\n");
		emit(f, "\t\t.lipcrel\t.cpy%sloop%d\n", p->z.v ? p->z.v->identifier : "null", loopid);
		emit(f, "\t\tadd\t%s\n", regnames[pc]);

	}
	// cleanup
	if(savec) {
		emit(f,"\tldinc\t%s\n",regnames[sp]);
		emit(f,"\tmr\t%s\n",regnames[cntr]);
		pushed-=4;
	}
	if(saved) {
		emit(f,"\tldinc\t%s\n",regnames[sp]);
		emit(f,"\tmr\t%s\n",regnames[dstr]);
		pushed-=4;
	}
	cleartempobj(f,t1);
	cleartempobj(f,tmp);
	loopid++;
}


/* Similar to inlinememcpy, but copies an array or struct to the stack */

void emit_inlinepush(FILE *f,struct IC *p, int t)
{
	int srcr = t1;
	int dstr=0;
	int cntr=0;
	int savec=1;
	int saved=0;
	int wordcopy;
	int bytecopy;
	zmax copysize = opsize(p);
	int unrollwords=0;
	int unrollbytes=0;

	// Can we create larger code in the interests of speed?  If so, partially unroll the copy.
	wordcopy = copysize & ~3;
	bytecopy = copysize - wordcopy;

	if ((wordcopy < 32 && !optsize) || (wordcopy<16))
		unrollwords=1;
	if (bytecopy < 5)
		unrollbytes=1;

	cleartempobj(f,t1);
	cleartempobj(f,tmp);

	// Even if a register is available we still have to save it because the current function wouldn't
	// but the parent function may be using it.  Therefore we might as well use a hardcoded register.
	dstr=sp;

	// FIXME - don't necessarily need the counter register if the copy is small...

	/* Is our source a register?  If so, set srcr accordingly */
	if (p->q1.flags & REG) {
		srcr=p->q1.reg;
		cntr=t1;
	} else
		cntr=t1+2;

	if((unrollwords && unrollbytes) || regs[cntr]==0)
		savec=0;
	else {
		emit(f,"\tmt\t%s\n",regnames[cntr]);
		emit(f,"\tstdec\t%s\n",regnames[sp]);
		pushed+=4;
	}

	emit(f, "\t\t\t\t\t// Copying %d words and %d bytes to stack\n", wordcopy / 4, bytecopy);

//	if(!p->z.v)
//		printf("No z->v: z flags: %x\n",p->z.flags);

	// Prepare the copy
	emit_prepobj(f, &p->q1, t,srcr,0);

	// Make room on the stack for the copied object

	emit(f,"\t.liconst\t%d\n",pushsize(p));
	emit(f,"\tsub\t%s\n",regnames[sp]);

	if (unrollwords) {
		wordcopy >>= 2;
		if (wordcopy) {
			emit(f, "\t\t\t\t\t// Copying %d words to stack\n", wordcopy);
		}
		while (wordcopy--) {
			emit(f, "\tldinc\t%s\n\tstinc\t%s\n", regnames[srcr], regnames[dstr]);
		}
	} else {
		emit(f, "\t\t\t\t\t// Copying %d words to stack\n", wordcopy / 4);
		// Copy bytes...
		emit_constanttotemp(f, wordcopy);
		emit(f, "\taddt\t%s\n", regnames[dstr]);
		emit(f, "\tmr\t%s\n", regnames[cntr]);
		emit(f, ".cpystackwordloop%d:\n", loopid);
		emit(f, "\tldinc\t%s\n\tstinc\t%s\n", regnames[srcr], regnames[dstr]);
		emit(f, "\tmt\t%s\n\tcmp\t%s\n", regnames[dstr], regnames[cntr]);
		emit(f, "\tcond\tNEQ\n");
		emit(f, "\t\t.lipcrel\t.cpystackwordloop%d\n", loopid);
		emit(f, "\t\tadd\t%s\n", regnames[pc]);
	}

	if (unrollbytes) {
		if (bytecopy)
			emit(f, "\t\t\t\t\t// Copying %d byte tail to stack\n", bytecopy);
		while (bytecopy--)
			emit(f, "\tldbinc\t%s\n\tstbinc\t%s\n", regnames[srcr], regnames[dstr]);
	} else {
		emit(f, "\t\t\t\t\t// Copying %d bytes to stack\n", bytecopy);
		// Copy bytes...
		emit_constanttotemp(f, bytecopy);
		emit(f, "\taddt\t%s\n", regnames[dstr]);
		emit(f, "\tmr\t%s\n", regnames[cntr]);
		emit(f, ".cpystackloop%d:\n", loopid);
		emit(f, "\tldbinc\t%s\n\tstbinc\t%s\n", regnames[srcr], regnames[dstr]);
		emit(f, "\tmt\t%s\n\tcmp\t%s\n", regnames[dstr], regnames[cntr]);
		emit(f, "\tcond\tNEQ\n");
		emit(f, "\t\t.lipcrel\t.cpystackloop%d\n", loopid);
		emit(f, "\t\tadd\t%s\n", regnames[pc]);

	}
	// cleanup

	// Reset the stack pointer since we've been blithely post-incrementing it.
	emit(f,"\t.liconst\t%d\n",opsize(p));
	emit(f,"\tsub\t%s\n",regnames[sp]);

	if(savec) {
		emit(f,"\tldinc\t%s\n",regnames[sp]);
		emit(f,"\tmr\t%s\n",regnames[cntr]);
		pushed-=4;
	}
	cleartempobj(f,t1);
	cleartempobj(f,tmp);
	loopid++;
}

