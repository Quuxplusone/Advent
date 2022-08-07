/*
To do: 
	Peephole optimise away such constructs as mr r0, mt r0
	| Optimise addresses of stack variables - lea's can mostly be replaced with simple adds.
	Detect absolute moves to reg, prune any that aren't needed.

	tempreg logic should be correct - now it's up to machine.c to make good use of it.
*/

zmax val2zmax(struct obj *o, int t)
{
	union atyps *p = &o->val;
	t &= NU;
	if (t == CHAR)
		return (zc2zm(p->vchar));
	if (t == (UNSIGNED | CHAR))
		return (zuc2zum(p->vuchar));
	if (t == SHORT)
		return (zs2zm(p->vshort));
	if (t == (UNSIGNED | SHORT))
		return (zus2zum(p->vushort));

	/*
	   if(t==FLOAT) return(zf2zld(p->vfloat);emitzld(f,vldouble);}
	   if(t==DOUBLE){vldouble=zd2zld(p->vdouble);emitzld(f,vldouble);}
	   if(t==LDOUBLE){emitzld(f,p->vldouble);}
	 */

	if (t == INT)
		return (zi2zm(p->vint));
	if (t == (UNSIGNED | INT))
		return (zui2zum(p->vuint));
	if (t == LONG)
		return (zl2zm(p->vlong));
	if (t == (UNSIGNED | LONG))
		return (zul2zum(p->vulong));
	if (t == LLONG)
		return (zll2zm(p->vllong));
	if (t == (UNSIGNED | LLONG))
		return (zull2zum(p->vullong));
	if (t == MAXINT)
		return (p->vmax);
	if (t == (UNSIGNED | MAXINT))
		return (p->vumax);
	if (t == POINTER)
		return (zul2zum(p->vulong));
	printf("#FIXME - no float support yet\n");
	ierror(0);
}

static void emit_sizemod(FILE * f, int type)
{
	if(DBGMSG)
		emit(f, "\t\t\t\t\t\t//sizemod based on type 0x%x\n", type);
	switch (type & NQ) {
	case 0:
		if(DBGMSG)
			emit(f, "\t\t\t\t\t\t//sizemod type is zero (movefromreg?)");
		break;
	case CHAR:
		emit(f, "\tbyt\n");
		break;
	case SHORT:
		emit(f, "\thlf\n");
		break;
	case INT:
	case LONG:
	case LLONG:
	case POINTER:
	case FUNKT:		// Function pointers are dereferenced by calling them.
	case STRUCT:
	case UNION:
	case ARRAY:
		break;		// Structs and unions have to remain as pointers
	default:
		printf("emit_sizemod - type %d not handled\n", type);
		ierror(0);
		break;
	}
}


// WARNING: Must invalidate tmp if control flow doesn't change immediately after this instruction.

static void emit_pcreltotemp(FILE * f, char *lab, int suffix)
{
	int i;
	if(DBGMSG)
		emit(f, "\t\t\t\t\t\t//pcreltotemp\n");
	emit(f, "\t.lipcrel\t%s%d\n", lab, suffix);
//	cleartempobj(f,tmp);
}

static void emit_pcreltotemp2(FILE *f,struct obj *p)
{
	if(DBGMSG)
		emit(f, "\t\t\t\t\t\t//pcreltotemp\n");
	if (p->v->storage_class == STATIC)
		emit(f,"\t.lipcrel\t%s%d\n",labprefix, zm2l(p->v->offset));
	else if(p->v->storage_class == EXTERN)
	{
		if(p->val.vmax)
			emit(f,"\t.lipcrel\t_%s%d\n",p->v->identifier, p->val.vmax);
		else
			emit(f,"\t.lipcrel\t_%s\n",p->v->identifier);
	}
//	cleartempobj(f,tmp);
}

// tempobj logic should be correct.

static void emit_externtotemp(FILE * f, char *lab, int offset)	// FIXME - need to find a way to do this PC-relative.
{
#if 0
	emit(f, "\tldinc\t%s\n", regnames[pc]);
	if (offset)
		emit(f, "\t.ref\t_%s, %d\n",lab, offset);
	else
		emit(f, "\t.ref\t_%s\n",lab);
#else
	if(g_flags[FLAG_PIC]&USEDFLAG)
	{
		if (offset)
			emit(f, "\t.lipcrel\t_%s, %d\n",lab, offset);
		else
			emit(f, "\t.lipcrel\t_%s\n",lab);
		emit(f, "\taddt\t%s\n",regnames[pc]);
	}
	else
	{
		if (offset)
			emit(f, "\t.liabs\t_%s, %d\n",lab, offset);
		else
			emit(f, "\t.liabs\t_%s\n",lab);
	}
#endif
	cleartempobj(f,tmp);
}


// tempobj logic should be correct.

static void emit_statictotemp(FILE * f, char *lab, int suffix, int offset)	// FIXME - need to find a way to do this PC relative
{
	if(DBGMSG)
		emit(f, "\t\t\t\t\t\t//statictotemp (FIXME - make PC-relative?)\n");
	if(g_flags[FLAG_PIC]&USEDFLAG)
	{
		emit(f, "\t.lipcrel\t%s%d,%d\n", lab, suffix, offset);
		emit(f, "\taddt\t%s\n",regnames[pc]);
	}
	else
	{
		emit(f, "\t.liabs\t%s%d,%d\n", lab, suffix, offset);
	}
	cleartempobj(f,tmp);
}

static int count_constantchunks(zmax v)
{
	int chunk = 1;
	int v2 = (int)v;
	while (((v2 & 0xffffffe0) != 0) && ((v2 & 0xffffffe0) != 0xffffffe0))	// Are we looking at a sign-extended 6-bit value yet?
	{
//               printf("%08x\n",v2);
		v2 >>= 6;
		++chunk;
	}
	return (chunk);
}


// tempobj logic should be correct.

static void emit_constanttoreg(FILE * f, zmax v,int reg)
{
	int matchreg=matchtempkonst(f,v,reg);
//	emit(f,"// matchreg %s\n",regnames[matchreg]);

//	if(matchreg==tmp)
//		return;
//	else if(matchreg)
//		emit(f,"\tmt\t%s\n",regnames[matchreg]);
	if(matchreg) {
		// Need to deal with the case where a constant is in r0 but required in tmp or vice versa.
		if(matchreg!=reg) {
			if(reg==tmp)
				emit(f,"\tmt\t%s\n",regnames[matchreg]);
			else
				emit(f,"\tmr\t%s\n",regnames[reg]);
			settempkonst(f,reg,v);
		}
	} else {
		emit(f, "\t.liconst\t%d\n", v);
		settempkonst(f,tmp,v);
		if(reg!=tmp)
		{
			emit(f,"\tmr\t%s\n",regnames[reg]);
			settempkonst(f,reg,v);
		}
	}
}

static void emit_constanttotemp(FILE * f, zmax v)
{
	emit_constanttoreg(f,v,tmp);
}


// tempobj logic should be correct.

static void emit_stackvartotemp(FILE * f, zmax offset, int deref)
{
	if (deref) {
		if (offset) {
			emit_constanttotemp(f, offset);
			emit(f, "\tldidx\t%s\n", regnames[sp]);
		} else
			emit(f, "\tld\t%s\n", regnames[sp]);
	} else {
		if (offset) {
			emit_constanttotemp(f, offset);
			emit(f, "\taddt\t%s\n", regnames[sp]);
		} else
			emit(f, "\tmt\t%s\n", regnames[sp]);
	}
	cleartempobj(f,tmp);
}


// Load the address of a target obj into reg in preparation for a store.
// If the target is simply a register then does nothing.
// The nominated register can be tmp or any gpr.
// Guaranteed not to modify t1 or t2 except when nominated.
// tempobj logic should be correct.

static void emit_prepobj(FILE * f, struct obj *p, int t, int reg, int offset)
{
	int matchreg=0;

	if(DBGMSG)
		emit(f, "\t\t\t\t\t\t// (prepobj %s)\n ", regnames[reg]);

	if (p->flags & REG) {
		if(DBGMSG)
			emit(f, "\t\t\t\t\t\t// reg %s - no need to prep\n", regnames[p->reg]);
		if(p->flags & DREFOBJ)
		{
			if (reg == tmp)
			{
				emit(f, "\tmt\t%s\n", regnames[p->reg]);
				cleartempobj(f,tmp);
			}
		}
		return;
	}

	if(!offset)
		matchreg=matchtempobj(f,p,1,t1);  // FIXME - we're hunting for varadr here.

	if(matchreg)
	{
		if(DBGMSG)
			emit(f,"\n\t\t\t\t\t\t// required value found in %s\n",regnames[matchreg]);
		if(matchreg==reg)
			return;
		else if(matchreg==tmp) {
			emit(f,"\tmr\t%s\n",regnames[reg]);
			settempobj(f,reg,p,0,0);
			return;
		} else {
			emit(f,"\tmt\t%s\n",regnames[matchreg]);
			settempobj(f,tmp,p,0,0);
			if(reg!=tmp)
			{
				emit(f,"\tmr\t%s\n",regnames[reg]);
				settempobj(f,reg,p,0,0);
			}
			return;
		}
	}

	if (p->flags & DREFOBJ) {
		if (p->flags & VARADR)
			if(DBGMSG)
				emit(f, "\t\t\t\t\t\t//varadr AND ");
		if(DBGMSG)
			emit(f, "\t\t\t\t\t\t// deref\n");
		/* Dereferencing a pointer */
		if (p->flags & KONST) {
			if(DBGMSG)
				emit(f, "\t\t\t\t\t\t// const to %s\n",regnames[reg]);
			emit_constanttoreg(f, val2zmax(p, p->dtyp) + offset,reg);
//			if (reg != tmp)
//				emit(f, "\tmr\t%s\n", regnames[reg]);
			settempkonst(f,reg,val2zmax(p, p->dtyp) + offset);
		} else if (p->flags & REG) {
			if (reg == tmp)
			{
				emit(f, "\tmt\t%s\n", regnames[p->reg]);
				cleartempobj(f,tmp);
			}
			else if(DBGMSG)
				emit(f, "\t\t\t\t\t\t// reg %s - no need to prep\n", regnames[p->reg]);
		} else if (p->flags & VAR) {
			if(DBGMSG)
				emit(f, "\t\t\t\t\t\t// var FIXME - deref?\n");
			if(offset)
			{
				printf("emit_prepobj: Offset supplied but object is being dereferenced!\n");
				ierror(0);
			}
			if (isauto(p->v->storage_class)) {
				if(DBGMSG)
					emit(f, "\t\t\t\t\t\t// reg - auto\n");
				emit_stackvartotemp(f, real_offset(p) + offset, 1);
				if (reg != tmp)
				{
					emit(f, "\tmr\t%s\n", regnames[reg]);
					cleartempobj(f,reg);
				}
			} else if (isstatic(p->v->storage_class)) {
				cleartempobj(f,tmp);
				cleartempobj(f,reg);
				if(DBGMSG)
					emit(f, "\t\t\t\t\t\t// static\n");
				emit(f, "\tldinc\tr7\n\t.ref\t%s%d,%d\n",
				     labprefix, zm2l(p->v->offset), offset + p->val.vmax);
				emit(f, "\tldt\n");
				if (reg != tmp)
					emit(f, "\tmr\t%s\n", regnames[reg]);
			} else if (isextern(p->v->storage_class)) {
				emit(f, "\t\t\t\t\t\t//Extern\n");
				emit_externtotemp(f, p->v->identifier, p->val.vmax);
				emit(f, "\tldt\n");
				if (reg != tmp)
					emit(f, "\tmr\t%s\n", regnames[reg]);
			} else {
				if(DBGMSG)
					printf("// emit_prepobj (deref): - unknown storage class!\n");
				ierror(0);
			}
//                              if(!zmeqto(l2zm(0L),p->val.vmax)){
//                                      emit(f," offset ");
//                                      emit(f," FIXME - deref?\n");
//                                      emit_constanttotemp(f,val2zmax(f,p,LONG));
//                                      emit(f,"\tmr\t%s\n",regnames[reg]);
//                                      emit_pcreltotemp(f,labprefix,zm2l(p->v->offset));
//                                      emit(f,"\tadd\t%s\n",regnames[reg]);
//                              }
		}
	} else {
		if (p->flags & REG) {
			if(DBGMSG)
				emit(f, "\t\t\t\t\t\t// reg %s - no need to prep\n", regnames[p->reg]);
		} else if (p->flags & VAR) {
			if (isauto(p->v->storage_class)) {
				/* Set a register to point to a stack-base variable. */
				if(DBGMSG)
					emit(f, "\t\t\t\t\t\t// var, auto|reg\n");
				if (p->v->storage_class == REGISTER)
					if(DBGMSG)
						emit(f, "\t\t\t\t\t\t// (is actually REGISTER)\n");
				emit_stackvartotemp(f, real_offset(p) + offset, 0);
				if (reg != tmp)
					emit(f, "\tmr\t%s\n\n", regnames[reg]);
			} else if (isextern(p->v->storage_class)) {
				if(DBGMSG)
					emit(f, "\t\t\t\t\t\t// extern (offset %d)\n", p->val.vmax);
				emit_externtotemp(f, p->v->identifier, p->val.vmax + offset);
				if(DBGMSG)
					emit(f, "\t\t\t\t\t\t// extern pe %s varadr\n", p->flags & VARADR ? "is" : "not");
				if (reg != tmp)
					emit(f, "\tmr\t%s\n", regnames[reg]);
			} else if (isstatic(p->v->storage_class)) {
				if(DBGMSG)
					emit(f, "\t\t\t\t\t\t// static\n");
//				emit(f, "\tldinc\tr7\n\t.ref\t%s%d,%d\n",
//				     labprefix, zm2l(p->v->offset), offset + p->val.vmax);
				if(g_flags[FLAG_PIC]&USEDFLAG)
				{
					emit(f, "\t.lipcrel\t%s%d,%d\n",
						 labprefix, zm2l(p->v->offset), offset + p->val.vmax);
					emit(f, "\taddt\t%s\n",regnames[pc]);
				}
				else
					emit(f, "\t.liabs\t%s%d,%d\n",
						 labprefix, zm2l(p->v->offset), offset + p->val.vmax);
				if(DBGMSG)
					emit(f, "\t\t\t\t\t\t// static pe %s varadr\n", p->flags & VARADR ? "is" : "not");
				if (reg != tmp)
					emit(f, "\tmr\t%s\n", regnames[reg]);
			} else {
				if(DBGMSG)
					printf("emit_prepobj: - unknown storage class!\n");
				ierror(0);
			}
			settempobj(f,tmp,p,0,1);
			settempobj(f,reg,p,0,1);
		}
	}
}


// Returns 1 if the Z flag has been set (i.e. a load has occurred)
// Guaranteed not to modify t1 or t2.

static int emit_objtoreg(FILE * f, struct obj *p, int t,int reg)
{
	int result=0;
	int matchreg;
	int elementary;
	int postinc=0;
	if(DBGMSG)
		emit(f, "\t\t\t\t\t\t// (obj to %s) flags %x type %x\n",regnames[reg],p->flags,t);

	/* If we're dealing with an elementary type on the stack we'll copy the actual object into
	   a register.  If we're dealing with a composite type, or taking the address of an elementary
	   type then instead we'll take its address into the register.  There's a subtlety to take care
	   of with ASSIGN ICs and inline memcpy/strcpy where they type can be CHAR with a size!=1.
	   I hack around this by overriding type in the parent fucction. */
	if ((!(p->flags & VARADR)) && ((t & NQ) != STRUCT) && ((t & NQ) != UNION) && ((t & NQ) != ARRAY))
		elementary=1;
	else
		elementary=0;

	matchreg=matchtempobj(f,p,0,reg);

	if ((p->flags & (REG|DREFOBJ)) == REG) {
		settempobj(f,reg,p,0,0);
		if(DBGMSG)
			emit(f, "\t\t\t\t\t\t// reg %s - only match against tmp\n", regnames[p->reg]);
		if (reg == p->reg)
			return(0);
		if(matchreg!=tmp)
		{
			emit(f,"\tmt\t%s\n",regnames[p->reg]);
			settempobj(f,tmp,p,0,0);
		}
		if(reg!=tmp)
		{
			emit(f, "\tmr\t%s\n", regnames[reg]);
			settempobj(f,reg,p,0,0);
		}
		return(0);
	}

	if(matchreg)
	{
		if(DBGMSG)
			emit(f,"\n\t\t\t// required value found in %s\n",regnames[matchreg]);
		if(matchreg==reg)
			return(0);
		if(matchreg!=tmp)
		{
			emit(f,"\tmt\t%s\n",regnames[matchreg]);
			settempobj(f,tmp,p,0,0);
		}
		if(reg!=tmp)
		{
			emit(f,"\tmr\t%s\n",regnames[reg]);
			settempobj(f,reg,p,0,0);
		}
		emit(f,"\t\t\t\t//return 0\n");
		return(0);
	}

	// FIXME - does this have implications for structs, unions, fptrs, etc?
	if(p->flags&VARADR)
	{
		emit_prepobj(f,p,t,reg,0);
		return(0);
	}
	if ((p->flags & (KONST | DREFOBJ)) == (KONST | DREFOBJ)) {
		if(DBGMSG)
			emit(f, "\t\t\t\t\t\t// const/deref\n");
		matchreg=matchtempkonst(f, val2zmax(p, p->dtyp),tmp);
		if(matchreg && matchreg!=tmp) {
			emit_sizemod(f, t);
			emit(f,"\tld\t%s\n",regnames[matchreg]);
		} else {
			emit_prepobj(f, p, t, tmp, 0);
			emit_sizemod(f, t);
			emit(f, "\tldt\n");
		}
		settempobj(f,tmp,p,0,0);
		settempobj(f,reg,p,0,0);
		if(reg!=tmp)
			emit(f,"\tmr\t%s\n",regnames[reg]);
		return(1);
	}

//	printf("p->flags %x, type %d\n",p->flags,t);

	if (p->flags & DREFOBJ) {
		if(DBGMSG)
			emit(f, "\t\t\t\t\t\t// deref \n");
		/* Dereferencing a pointer */
		if (p->flags & REG) {
			switch (t & NQ) {
			case CHAR:
				if (p->am && p->am->type == AM_POSTINC)
				{
					emit(f, "\tldbinc\t%s\n", regnames[p->reg]);
					postinc=1;
				}
				else if (p->am && p->am->disposable)
					emit(f,
					     "\tldbinc\t%s\n//Disposable, postinc doesn't matter.\n", regnames[p->reg]);
				else
					emit(f, "\tbyt\n\tld\t%s\n", regnames[p->reg]);
				break;
			case SHORT:
				emit(f, "\thlf\n");
				emit(f, "\tld\t%s\n", regnames[p->reg]);
				break;
			case INT:
			case LONG:
			case POINTER:
				if (p->am && p->am->type == AM_POSTINC)
				{
					emit(f, "\tldinc\t%s\n", regnames[p->reg]);
					postinc=4;
				}
				else
					emit(f, "\tld\t%s\n", regnames[p->reg]);
				break;
			case STRUCT:
			case UNION:
			case ARRAY:
			case FUNKT:	// Function pointers are dereferenced by calling them.
				emit(f, "\tmt\t%s\n", regnames[p->reg]);
				break;
			default:
				fprintf(stderr,"Objtoreg - unhandled type %d\n",t);
				ierror(0);
				break;
			}
			result=1;
		} else {
			emit_prepobj(f, p, t, tmp, 0);
			// Exclusions for fptrs, structs and unions to avoid double-dereference.
			// Included array type in these exclusions.  FIXME - is this sufficient?
			if ((t & NQ) != FUNKT && (t & NQ) != STRUCT && (t & NQ) != UNION && (t & NQ) != ARRAY)
			{
				emit_sizemod(f, t);
				emit(f, "\tldt\n");
				result=1;
			}
		}
	} else {
		if (p->flags & REG) {
			// Already handled in the preamble.
			if(DBGMSG)
				emit(f, "\t\t\t\t\t\t// reg %s\n", regnames[p->reg]);
			emit(f, "\tmt\t%s\n", regnames[p->reg]);
		} else if (p->flags & VAR) {
			if (isauto(p->v->storage_class)) {
				if(DBGMSG)
					emit(f, "\t\t\t\t\t\t// var, auto|reg\n");
				/* Do we need to deference? */
				if (elementary) {
					if (real_offset(p)) {
						emit_constanttotemp(f, real_offset(p));
						if(!isstackparam(p))
							emit_sizemod(f, t);
						emit(f, "\tldidx\t%s\n", regnames[sp]);
					} else {
						emit_sizemod(f, t);
						emit(f, "\tld\t%s\n", regnames[sp]);
					}
					result=1;
				} else {
					if (real_offset(p)) {
						emit_constanttotemp(f, real_offset(p));
						emit(f, "\taddt\t%s\n", regnames[sp]);
					} else {
						emit(f, "\tmt\t%s\n", regnames[sp]);
					}
				}
			} else if (isextern(p->v->storage_class)) {
				if(DBGMSG)
					emit(f, "\t\t\t\t\t\t// extern\n");
				emit_externtotemp(f, p->v->identifier, p->val.vmax);
				// Structs, unions and arrays have to remain as pointers
				if (elementary) {
					if(DBGMSG)
						emit(f, "\t\t\t\t\t\t//extern deref\n");
					emit_sizemod(f, t);
					emit(f, "\tldt\n");
					result=1;
				}
			} else if (isstatic(p->v->storage_class)) {
				if(DBGMSG)
					emit(f, "\t\t\t\t\t\t//static %s\n", p->flags & VARADR ? "varadr" : "not varadr");
				emit_statictotemp(f, labprefix, zm2l(p->v->offset), p->val.vmax);
				// Structs, unions and arrays have to remain as pointers
				if (elementary) {
					if(DBGMSG)
						emit(f, "\t\t\t\t\t\t//static deref\n");
					emit_sizemod(f, t);
					emit(f, "\tldt\n");
					result=1;
				}
			} else {
				printf("Objtotemp: Unhandled storage class: %d\n", p->v->storage_class);
				ierror(0);
			}
		} else if (p->flags & KONST) {
			if(DBGMSG)
				emit(f, "\t\t\t\t\t\t// const\n");
			emit_constanttotemp(f, val2zmax(p, t));
		} else {
			printf("Objtotemp: unknown flags %d\n", p->flags);
			ierror(0);
		}
	}
	if(reg!=tmp)
		emit(f,"\tmr\t%s\n",regnames[reg]);
	settempobj(f,reg,p,-postinc,0);
	settempobj(f,tmp,p,-postinc,0);
	return(result);
}

