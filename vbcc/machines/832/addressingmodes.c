/* search for possible addressing-modes */

/* DONE: Determine whether a given ob is disposable. */

/* ToDo: Look for pairs of ICs with Z set to register,
   followed by disposable q1 on the same register ->
   use tmp for that register.  (Provided second IC won't trash tmp)
   Can we avoid the passing register being allocated? */

/* Look for ways to use ldidx when an IC adds to a pointer and the next one dereferences it */


#define DEFERREDPOP_FLOWCONTROL 0
#define DEFERREDPOP_NESTEDCALLS 1
#define DEFERREDPOP_OK 2


#define AM_DEBUG 0

// If the obj doesn't already have an addressing mode, create one and zero it out.
static void am_alloc(struct obj *o)
{
	if (!o->am) {
		o->am = mymalloc(sizeof(struct AddressingMode));
		memset(o->am, 0, sizeof(struct AddressingMode));
	}
}


void am_deferredpop(struct IC *p)
{
	struct IC *p2;
	int pushed=0;
	int deferredpop=0;
	int candefer=0;
	for(;p;p=p->next)
	{
		switch(p->code)
		{
			case PUSH:
				pushed+=pushsize(p);
//				printf("Pushing %d bytes, %d so far\n",pushsize(p),pushed);
				break;
			case CALL:
//				printf("CALL IC with pushedsize %d\n",pushedargsize(p));

				am_alloc(&p->z);

				/* We detected nested function calls by comparing the amount of data pushed to the 
				   stack with this call's pushedargsize().  If they don't match we're dealing with
                   nested calls.  If they do match, we might be able to defer stack popping. */
				if(pushed==pushedargsize(p))
				{
//					printf("Checking for flow change\n");
					p2=p->next;
					candefer=1;
					while(p2)
					{
						switch(p2->code)
						{
							/* If we encounter LABEL, COMPARE, TEST or a branch operation then program flow changes
							   before the next PUSH, and we can't defer the stack pop. */
							case LABEL:
							case COMPARE:
							case TEST:
							case BLT:
							case BGT:
							case BLE:
							case BGE:
							case BEQ:
							case BNE:
							case BRA:
//								printf("Flow control changed - can't defer\n");
								p->z.am->deferredpop=DEFERREDPOP_FLOWCONTROL;
								candefer=0;
								p2=0;
								break;
							/* If we encounter another PUSH operation without a flow control change we can defer */
							case PUSH:
//								printf("Can defer stack popping\n");
								p2=0;
								break;
							default:
								p2=p2->next;
								break;
						}
					}
				}
				else /* pushedargsize() and the contents of the stack differed in size - nested calls. */
				{
					p->z.am->deferredpop=DEFERREDPOP_NESTEDCALLS;
//					printf("Nested call - can't defer stack popping\n");
					candefer=0;
				}

				if(candefer)
				{
					p->z.am->deferredpop=DEFERREDPOP_OK;
					deferredpop=pushedargsize(p);
				}

				pushed-=pushedargsize(p);
				break;
			default:
				break;
		}
	}
}


static int is_arithmetic_bitwise(int code)
{
	switch(code)
	{
		case ADD:
		case ADDI2P:
		case SUB:
		case SUBIFP:
		case MULT:
		case OR:
		case XOR:
		case AND:
		case LSHIFT:
		case RSHIFT:
			return(1);
			break;
	}
	return(0);
}


void am_conversions(struct IC *p)
{
	int c;
	struct IC *p2;
	struct IC *p3;
	for (; p; p = p->next) {
		c = p->code;
		switch(c)
		{
			case BEQ:
			case BNE:
			case BLT:
			case BLE:
			case BGT:
			case BGE:
			case ALLOCREG:
			case FREEREG:
			case NOP:
				break;
			case CONVERT:
				printf("(conversion)\n");
				break;

			default:
				switch(q1typ(p)&NQ)
				{
					case CHAR:
					case SHORT:
						printf("IC %d, non-int operation %x, %x\n",c,q1typ(p),ztyp(p));
					default:
						break;
				}
				break;
		}
	}
}

// Look for 832-specific optimizations

extern zmax sizetab[MAX_TYPE + 1];

void am_simplify(struct IC *p)
{
	int c;
	struct IC *p2;
	struct IC *p3;
	struct IC *prev;
	for (; p; p = p->next) {
		c = p->code;

		p2=p->next;
		if(!p2)
			break;
		p3=p2->next;
		if(!p3)
			break;

		switch(c)
		{
			// Check for ADDRESS followed by push - can we merge?
			case ADDRESS:
				if((p->z.flags&(REG|DREFOBJ|SCRATCH))!=(REG|SCRATCH))
					break;

//				printf("Address IC candidate: zreg: %d, z flags %x\n",p->z.reg,p->z.flags);

				while(p3)
				{
//					printf(" next op: %x, q1reg: %d, q1flags: %x, q2reg: %d, q2flags: %x\n",
//						p2->code,p2->q1.reg,p2->q1.flags,p2->q2.reg,p2->q2.flags);
					switch(p2->code)
					{
						case PUSH:
							if((p2->q1.flags&(REG|DREFOBJ|VARADR))==REG && p2->q1.reg==p->z.reg)
							{
//								printf("p3 code: %x, reg: %d\n",p3->code,p3->q1.reg);
								if(p3->code!=FREEREG || p3->q1.reg!=p->z.reg)
									break;
//								printf("Push: merging\n");
								p->code=NOP;	// Don't prepare the address to a register...
								p2->q1=p->q1;	// Push it directly.
								p2->q1.flags|=VARADR;
								p3=0;
							}
							break;
						case FREEREG:
							if(p2->q1.reg==p->z.reg)
							{
//								printf("Freereg: bailing out\n");
								p3=0;
							}
							break;
						default:
							break;
					}
					// Does the IC reference the register?
					if((p2->q1.flags&REG) && p2->q1.reg==p->z.reg)
						p3=0;
					else if((p2->q2.flags&REG) && p2->q2.reg==p->z.reg)
						p3=0;
					p2=p3;
					if(p3)
						p3=p3->next;
				}
				break;

			// Check for GETRETURN followed by TEST/COMPARE of call result which is then discarded...
			case GETRETURN:
				if((p->z.flags&(REG|DREFOBJ|SCRATCH))!=(REG|SCRATCH))
					break;

				// FIXME - can do the same for other instructions, provided we don't need to use r0 to derive the second op.

				if(p3->code!=FREEREG || p3->q1.reg!=p->z.reg)
					break;

				switch(p2->code)
				{
					case TEST:
						if(p2->q1.flags&(REG|DREFOBJ)!=REG || p2->q1.reg!=p->z.reg)
							break;
						p->code=NOP;	// Don't bother retrieving the return value...
						p2->q1.reg=p->q1.reg;	// Test it directly from return register.
						break;
					case SETRETURN:
						if(p2->q1.flags&(REG|DREFOBJ)!=REG || p2->q1.reg!=p->z.reg)
							break;
						p->code=NOP;	// Don't bother retrieving the return value...
						p2->q1.reg=p->q1.reg;
						break;
					case COMPARE:
						if((p2->q1.flags&(REG|DREFOBJ))==REG && p2->q1.reg==p->z.reg)
						{
							// Make sure the other op is simple enough not to need r0: FIXME - can we improve this?
//							if((p2->q2.flags&(REG|DREFOBJ))==REG || p2->q2.flags&KONST)
//							{
								// COMPARE is guaranteed not to touch t1 (r0).
								p->code=NOP;	// Don't bother retrieving the return value...
								p2->q1.reg=p->q1.reg;	// Test it directly from return register.
//							}
						}
						else if((p2->q2.flags&(REG|DREFOBJ))==REG && p2->q2.reg==p->z.reg)
						{
//							if((p2->q1.flags&(REG|DREFOBJ))==REG || p2->q1.flags&KONST)
//							{
								// COMPARE is guaranteed not to touch t1 (r0).
								p->code=NOP;	// Don't bother retrieving the return value...
								p2->q2.reg=p->q1.reg;	// Test it directly from return register.
//							}
						}
						break;
				}
				break;

			// Loads cause automatic zero-extension so can avoid conversion
			// in that case.  Arithmetic functions may require conversion - with the exception of AND and (unsigned) SHR,
			// since neither can result in extra set bits in the MSBs.
			case CONVERT:
				if(p2->code==CONVERT)
				{
//					printf("Found successive Convert ICs\n");
//					printf("p1: %x, %d, %x -> %x, p2: %x, %d, %x -> %x\n",p->z.flags,p->z.reg,q1typ(p),ztyp(p),
//						p2->q1.flags,p2->q1.reg,q1typ(p2),ztyp(p2));
					if((p->z.flags&(REG|DREFOBJ))==REG && ((p2->q1.flags&(REG|DREFOBJ))==REG) && p->z.reg==p2->q1.reg)
					{
//						printf("Register match\n");
						if(((q1typ(p)&NQ)<(ztyp(p)&NQ)) && (q1typ(p)==ztyp(p2)))
						{
//							printf("Sizes match - nullifying conversion to wider type.\n");
							p->code=NOP;
							p2->q1=p->q1;
						}
					}
				} else {
					if (AM_DEBUG)
						printf("Checking conversion from %d to %d, %d to %d\n",q1typ(p),ztyp(p),p->q1.reg,p->z.reg);
					/* Eliminate conversions to wider types within a single register if the value is unsigned */
					if((p->z.flags&(REG|DREFOBJ))==REG && ((p->q1.flags&(REG|DREFOBJ))==REG) && p->z.reg==p->q1.reg) {
						if((q1typ(p)&UNSIGNED) && (sizetab[q1typ(p) & NQ] < sizetab[ztyp(p) & NQ])) {
							if (AM_DEBUG)
								printf("Eliminating unnecessary conversion\n");
							p->code=NOP;
						}
					}
				}
//				printic(stdout,p);
				break;

			// Look for situations where an arithmetic operation will have set flags,
			// rendering an explicit TEST unnecessary.
			case TEST:
				prev=p->prev;
				if(prev)
				{
//					printf("TEST found, evaluating previous IC\n");
					if(is_arithmetic_bitwise(prev->code))
					{
//							printf("Arithmetic / bitwise IC found\n");
//							printf("%x, %d, %x, %d\n",prev->z.flags,prev->z.reg,p->q1.flags,p->q1.reg);
						if((prev->z.flags&(REG|DREFOBJ))==REG && ((p->q1.flags&(REG|DREFOBJ))==REG) && prev->z.reg==p->q1.reg)
						{
//								printf("Register match\n");
							p->code=NOP;
						}
					}
				}
				break;

			default:
				// Look for cases where multiple arithmetic / bitwise instructions are chained and could
				// avoid writing to an intermediate register...
				if(is_arithmetic_bitwise(c) && is_arithmetic_bitwise(p2->code) && p3->code==FREEREG)
				{
					if (AM_DEBUG)
						printf("Evaluating pair of arithmetic ops followed by freereg...\n");
					if(((p->z.flags&(REG|DREFOBJ))==REG) && ((p2->q1.flags&(REG|DREFOBJ))==REG) && ((p2->z.flags&(REG|DREFOBJ))==REG))
					{
						if (AM_DEBUG)
							printf("Ops are all register based...\n");
						if (AM_DEBUG)
							printf("p1.q1: %s, p1.q2: %s, p1.z: %s  -  p2.q1: %s, p2.q2: %s, p2.z: %s\n",
								regnames[p->q1.reg],regnames[p->q2.reg],regnames[p->z.reg],
								regnames[p2->q1.reg],regnames[p2->q2.reg],regnames[p2->z.reg]);
						if(p2->q1.reg==p3->q1.reg && p->z.reg==p2->q1.reg && p2->q1.reg!=p2->z.reg && p2->q2.reg!=p2->z.reg)
						{
							if (AM_DEBUG)
								printf("Freereg matches - adjusting\n");
							p->z.reg=p2->z.reg;
							p2->q1.reg=p2->z.reg;
						}
					}
					// FIXME - explore the lifetime of the register we've just avoided; can we
					// avoid allocating / freeing it?  Won't help the code-generator but might be
					// able to avoid saving / restoring it in the function head / tail.
				}
				else
				{
					// Look specificially for addt candidates...
					if(c==ADD || c==ADDI2P)
					{
						int zr=(p->z.flags&(REG|DREFOBJ))==REG ? 1 : 0;

//						printf("add %x, %x, %x, %s, %s, %s\n",p->q1.flags,p->q2.flags,p->z.flags,
//								regnames[p->q1.reg],regnames[p->q2.reg],regnames[p->z.reg]);
						if((p->q1.flags&(REG|DREFOBJ))==REG)
						{
							if(!zr || p->z.reg!=p->q1.reg)
							{
								if((p->q2.flags&(REG|DREFOBJ))==REG)  // reg + reg => reg
								{
									// All three operands are registers - are they all different?
									if((zr || p->z.reg!=p->q2.reg) && p->q1.reg!=p->q2.reg)
									{
										am_alloc(&p->q1);
										p->q1.am->type=AM_ADDT;
										if (AM_DEBUG)
											printf("Marked addt candidate\n");
									}
								}
								if((p->q2.flags&(KONST|DREFOBJ))==KONST) // reg + const => reg
								{
									am_alloc(&p->q1);
									p->q1.am->type=AM_ADDT;
									if (AM_DEBUG)
									{
										printf("Marked addt candidate\n");
										printf("Reg + Konst => Reg\n");
									}
								}
							}
						}
					}
				}
				break;
		}
	}
}


void am_disposable(struct IC *p, struct obj *o)
{
	struct IC *p2;
	int disposable = 0;
	if (o->flags & REG) {
		// Cover the case where q1 or q2 are the same register as z.
		if(o!=&p->z)
		{
			// We're writing to the same register as we're reading from - not disposable!
			if((p->z.flags&&REG) && p->z.reg==o->reg)
				return;
		}
		p2 = p->next;
		while (p2) {
			if (p2->code == FREEREG && p2->q1.reg == o->reg) {
				if (AM_DEBUG)
					printf("\t(%s disposable.)\n", regnames[o->reg]);
				am_alloc(o);
				o->am->disposable = 1;
				return;
			}
			if (((p2->q1.flags & REG)
			     && p2->q1.reg == o->reg)
			    || ((p2->q2.flags & REG)
				&& p2->q2.reg == o->reg)
			    || ((p2->z.flags & REG)
				&& p2->z.reg == o->reg)) {
				//Found another instruction referencing reg - not disposable.
				return;
			}
			switch (p2->code) {
			case CALL:
			case BEQ:
			case BNE:
			case BLT:
			case BGE:
			case BLE:
			case BGT:
			case BRA:
				// Control flow changed, erring on the side of safety - not disposable.
				return;
				break;
			default:
				break;
			}
			p2 = p2->next;
		}
	}
}

struct IC *am_find_adjustment(struct IC *p, int reg)
{
	struct IC *p2 = p->next;
	// FIXME - limit how many steps we check...
	/* Look for a post-increment */
	while (p2) {
//		printf("\t\tChecking code %d\n",p2->code);
		if (p2->code == ADDI2P) {
			if (AM_DEBUG)
				printf("\tFound Addi2p to register %s \n", regnames[p2->z.reg]);
			if ((p2->q2.flags & KONST) && (p2->z.flags & REG)) {
				if (p2->z.reg == reg) {
					if (AM_DEBUG)
						printf("\t\tAdjusting the correct register - match found\n");
					break;
				} else {
					if(p2 && p2->q1.reg == reg) {	
						if (AM_DEBUG)
							printf("\t\tWriting to different reg - is source reg disposable?  ");
						am_disposable(p2, &p2->q1);
						if(p2->q1.am && p2->q1.am->disposable){
							if (AM_DEBUG)
								printf("yes\n");
							break;
						} else {
							if (AM_DEBUG)
								printf("no - bailing out\n");
							p2=0;
						}
					} else {
						if (AM_DEBUG)
							printf("\t\tWrong register - keep looking\n");
					}
				}
			} else {
				if (AM_DEBUG)
					printf("\t\tnot a constant, however - bailing out.\n");
				p2 = 0;
			}
		} else if (((p2->q1.flags & (REG | DREFOBJ)) && p2->q1.reg == reg)
			   || ((p2->q2.flags & (REG | DREFOBJ))
			       && p2->q2.reg == reg)
			   || ((p2->z.flags & (REG | DREFOBJ))
			       && p2->z.reg == reg)) {
			if (AM_DEBUG)
				printf("\t\tFound another instruction referencing reg - bailing out\n");
			p2 = 0;
		} else if (p2->code>=BEQ && p2->code<=BRA) {
			if (AM_DEBUG)
				printf("\t\tFound a branch instruction - bailing out\n");
			p2 = 0;
		} else if (p2->code>=LABEL) {
			if (AM_DEBUG)
				printf("\t\tFound a label - bailing out\n");
			p2 = 0;
		}
		if (p2)
			p2 = p2->next;
	}
	if (p2)
		return (p2);
	if (AM_DEBUG)
		printf("\tNo postincrements found - checking for predecrements\n");
	/* Search for a predecrement */
	p2 = p->prev;
	while (p2) {
		if (p2->code == SUBIFP) {
			if (AM_DEBUG)
				printf("\t\tFound subifp to register %s \n", regnames[p2->z.reg]);
			if ((p2->q2.flags & KONST) && (p2->z.flags & REG)) {
				if (p2 && p2->z.reg == reg) {
					if (AM_DEBUG)
						printf("\t\tAdjusting the correct register - match found\n");
					break;
				} else {
					if(p2 && p2->q1.reg == reg) {	
						if (AM_DEBUG)
							printf("\t\tWriting to different reg - is source reg disposable?  ");
						am_disposable(p2, &p2->q1);
						if(p2->q1.am && p2->q1.am->disposable){
							if (AM_DEBUG)
								printf("yes\n");
							break;
						} else {
							if (AM_DEBUG)
								printf("no - bailing out\n");
						}
					}
				}
			} else {
				if (AM_DEBUG)
					printf("\t\tnot a constant, however - bailing out.\n");
				p2 = 0;
			}
		} else if (((p2->q1.flags & (REG | DREFOBJ)) && p2->q1.reg == reg)
			   || ((p2->q2.flags & (REG | DREFOBJ))
			       && p2->q2.reg == reg)
			   || ((p2->z.flags & (REG | DREFOBJ))
			       && p2->z.reg == reg)) {
			if (AM_DEBUG)
				printf("\t\tFound another instruction referencing reg - bailing out\n");
			p2 = 0;
		}
		// FIXME - check for control flow changes
		if (p2)
			p2 = p2->prev;
	}
	return (p2);
}

// If we have a candidate for pre/post increment/decrement, validate that we can use it,
// and return the offset.  Zero if we can't use it.
int am_get_adjvalue(struct IC *p, int type, int target)
{
	int offset = p->q2.val.vmax;
	if (p->code == SUBIFP)
		offset = -offset;
	if (AM_DEBUG)
		printf("Offset is %d, type is %d, writing to target? %s\n", offset, type & NQ, target ? "yes" : "no");
	// Validate offset against type and CPU's capabilities.
	switch (type & NQ) {
	case CHAR:		// We only support postincrement for CHARs
		if (offset != 1)
			offset = 0;
		if (p->code == SUBIFP)
			offset = 0;
		break;
	case INT:
	case LONG:
	case POINTER:		// We support post-increment and predecrement for INTs/LONGs/PTRs
//                      if(target && offset!=-4)        // We only support predec for writing.
		if (target && ((offset != -4) && (offset != 4)))	// We now support predec and postinc for writing.
			offset = 0;
		if (!target && offset != 4)	// We only support postinc for reading
			offset = 0;
		if (p->code == ADDI2P && offset != 4)
			offset = 0;
		if (p->code == SUBIFP && offset != -4)
			offset = 0;
		break;
	case SHORT:		// We don't support either mode for shorts or anything else.
	default:
		offset = 0;
	}
	if (AM_DEBUG)
		printf("Validated offset is %d\n", offset);
	return (offset);
}

void am_prepost_incdec(struct IC *p, struct obj *o)
{
	struct IC *p2 = 0;
	int type;

	if (o->flags & (REG) && (o->flags & DREFOBJ)) {
		if (AM_DEBUG)
			printf("Dereferencing register %s - searching for adjustments\n", regnames[o->reg]);
		p2 = am_find_adjustment(p, o->reg);

		if (p2)		// Did we find a candidate for postinc / predec?
		{
			int adj;
			switch (p->code) {
			case CONVERT:
				// Are we considering q1 or z?
				if(o==&p->z)
					type = p->typf;
				else
					type = p->typf2;
				if (AM_DEBUG)
					printf("\tConvert operation - type is %d\n", type);
				break;
			default:
				if (AM_DEBUG)
					printf("\tRegular operation - type is %d\n", p->typf);
				type = p->typf;
				break;
			}
			adj = am_get_adjvalue(p2, type, o == &p->z);	// Validate the adjustment
			if (adj) {
				p2->code = NOP;	// Nullify the manual adjustment if we can do it as an opcode side-effect
				am_alloc(o);
				o->am->type = (adj > 0) ? AM_POSTINC : AM_PREDEC;
			}
		}
	}
}

#define getreg(x) (x.flags&REG ? x.reg : 0)

static void find_addressingmodes(struct IC *p)
{
	int c;
	struct obj *o;
	struct AddressingMode *am;

	am_deferredpop(p);
	am_simplify(p);
//	am_conversions(p);

	for (; p; p = p->next) {
		c = p->code;

		// Have to make sure that operands are different registers!
		if ((getreg(p->q1) == getreg(p->q2))
		    || (getreg(p->q1) == getreg(p->z))) {
			if (getreg(p->q1))
				if (AM_DEBUG)
					printf("Collision between q1 and q2 or z - ignoring\n");
		} else
			am_prepost_incdec(p, &p->q1);

		if ((getreg(p->q1) == getreg(p->q2))
		    || (getreg(p->q2) == getreg(p->z))) {
			if (getreg(p->q1))
				if (AM_DEBUG)
					printf("Collision between q2 and q1 or z - ignoring\n");
		} else
			am_prepost_incdec(p, &p->q2);

		if ((getreg(p->q1) == getreg(p->z))
		    || (getreg(p->q2) == getreg(p->z))) {
			if (getreg(p->z))
				if (AM_DEBUG)
					printf("Collision between z and q1 or q2 - ignoring\n");
		} else
			am_prepost_incdec(p, &p->z);
//		printic(stdout,p);

		am_disposable(p, &p->q1);
		am_disposable(p, &p->q2);
		am_disposable(p, &p->z);
	}
}

