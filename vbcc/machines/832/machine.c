/*  EightThirtyTwo backend for vbcc,
	based on the generic RISC backend

	The CPU targeted by this backend, and the latest version, can be found at
    https://github.com/robinsonb5/EightThirtyTwo

*/

// DONE - T2 no longer used at all - frees up a register for the main code generator

// DONE Update the memcpy code to save/allocate registers if needed.
// DONE Update the div code likewise.

// Complete the work on object tracking.  In particular take care of tracking an object vs
// its address.
// Also track via addressing-mode analysis whether or not it's valuable to save a value;
// Values can also be saved to otherwise unused registers.  (The compiler is almost certainly
// already smart enough to make use of unused registers for this, however!)

// DONE: eliminate unnecessary register shuffling for compare.

// DONE: Implement block copying

// DONE: Implement division / modulo using library code.
// DONE: Mark registers as disposable if their contents are never used beyond the current op.

// Look at ways of improving code efficiency.  Look for situations where the output of one IC
// becomes the input of another?  Would make a big difference when registers are all in use.

// Minus could be optimised for the in-register case.

// DONE: Do we need to reserve two temp registers?  Turns out one was sufficient, and giving
// the code generator an extra one to play with helped a great deal.

// Restrict byte and halfword storage to static and extern types, not stack-based variables.
// (Having learned more, bytes and halfwords on the stack are fine, the complication is with
// function parameters, which are promoted to int - thus the size modifier will be different
// for parameters and local variables even though both live on the stack.)

// DONE - Avoid moving registers for cmp and test when possible.

// Condition code for test may well be already set by previous load.
// Done for TEST, do the same for comparisons?

// Deal with dereferencing in temp caching - can we avoid repeated setups in tmp, maybe using r0?


#include "supp.h"

#define DBGMSG 1

static char FILE_[] = __FILE__;

/*  Public data that MUST be there.                             */

/* Name and copyright. */
char cg_copyright[] =
    "vbcc EightThirtyTwo code-generator, (c) 2019/2020 by Alastair M. Robinson\nBased on the generic RISC example backend (c) 2001 by Volker Barthelmann";

/*  Commandline-flags the code-generator accepts:
    0: just a flag
    VALFLAG: a value must be specified
    STRINGFLAG: a string can be specified
    FUNCFLAG: a function will be called
    apart from FUNCFLAG, all other versions can only be specified once */
int g_flags[MAXGF] = { 0 };

/* the flag-name, do not use names beginning with l, L, I, D or U, because
   they collide with the frontend */
/* 832-specific flags, "fpic" enables position independent code - name chosen to match gcc */
char *g_flags_name[MAXGF] = { "fpic" };

#define FLAG_PIC 0

/* the results of parsing the command-line-flags will be stored here */
union ppi g_flags_val[MAXGF] = { 0 };

/*  Alignment-requirements for all types in bytes.              */
zmax align[MAX_TYPE + 1];

/*  Alignment that is sufficient for every object.              */
zmax maxalign;

/*  CHAR_BIT for the target machine.                            */
zmax char_bit;

/*  sizes of the basic types (in bytes) */
zmax sizetab[MAX_TYPE + 1];

/*  Minimum and Maximum values each type can have.              */
/*  Must be initialized in init_cg().                           */
zmax t_min[MAX_TYPE + 1];
zumax t_max[MAX_TYPE + 1];
zumax tu_max[MAX_TYPE + 1];

/*  Names of all registers. will be initialized in init_cg(),
    register number 0 is invalid, valid registers start at 1 */
char *regnames[MAXR + 1];

/*  The Size of each register in bytes.                         */
zmax regsize[MAXR + 1];

/*  a type which can store each register. */
struct Typ *regtype[MAXR + 1];

/*  regsa[reg]!=0 if a certain register is allocated and should */
/*  not be used by the compiler pass.                           */
int regsa[MAXR + 1];

/*  Specifies which registers may be scratched by functions.    */
int regscratch[MAXR + 1];

/* specifies the priority for the register-allocator, if the same
   estimated cost-saving can be obtained by several registers, the
   one with the highest priority will be used */
int reg_prio[MAXR + 1];

/* an empty reg-handle representing initial state */
struct reg_handle empty_reg_handle = { 0, 0 };

/* Names of target-specific variable attributes.                */
char *g_attr_name[] = { "__interrupt", "__ctor", "__dtor", "__weak", 0 };

/****************************************/
/*  Private data and functions.         */
/****************************************/

#define USE_COMMONS 0

/* alignment of basic data-types, used to initialize align[] */
/* In actual fact 832 has full load/store alignment so this is negotiable based on -speed / -size flags. */
static long malign[MAX_TYPE + 1] = { 1, 1, 2, 4, 4, 4, 4, 8, 8, 1, 4, 1, 1, 1, 4, 1 };

/* sizes of basic data-types, used to initialize sizetab[] */
static long msizetab[MAX_TYPE + 1] = { 1, 1, 2, 4, 4, 8, 4, 8, 8, 0, 4, 0, 0, 0, 4, 0 };

/* used to initialize regtyp[] */
static struct Typ ltyp = { LONG }, ldbl = {
DOUBLE}, lchar = {
CHAR};

/* macros defined by the backend */
static char *marray[] = { "__section(x)=__vattr(\"section(\"#x\")\")",
	"__EIGHTTHIRTYTWO__",
	"__constructor(pri)=__vattr(\"ctor(\"#pri\")\")",
	"__destructor(pri)=__vattr(\"dtor(\"#pri\")\")",
	"__weak=__vattr(\"weak\")",
	0
};

/* special registers */
static int pc;			/*  Program counter                     */
static int sp;			/*  Stackpointer                        */
static int tmp;
static int t1, t2;		/*  temporary gprs */
static int f1, f2, f3;		/*  temporary fprs */
static int loopid = 0;	/* must be unique for every function in a compilation unit */


/* sections */
#define DATA 0
#define BSS 1
#define CODE 2
#define RODATA 3
#define SPECIAL 4

//static long stack;
static int section = -1, newobj;
static char *codename = "\t.section\t.text";
static char *dataname = "\t.section\t.data";
static char *bssname = "\t.section\t.bss";
static char *rodataname = "\t.section\t.rodata";
static int sectionid=0;

/* assembly-prefixes for labels and external identifiers */
static char *labprefix = "l", *idprefix = "_";

/* variables to keep track of the current stack-offset in the case of
   a moving stack-pointer */
static long pushed;
static long notyetpopped;

static long localsize, rsavesize, argsize;

static int count_constantchunks(zmax v);
static void emit_constanttotemp(FILE * f, zmax v);
static void emit_statictotemp(FILE * f, char *lab, int suffix, int offset);
static void emit_externtotemp(FILE * f, char *lab, int offset);
static void emit_pcreltotemp2(FILE *f,struct obj *p);

static void emit_prepobj(FILE * f, struct obj *p, int t, int reg, int offset);
static int emit_objtoreg(FILE * f, struct obj *p, int t,int reg);

/* calculate the actual current offset of an object relativ to the
   stack-pointer; we use a layout like this:
   ------------------------------------------------
   | arguments to this function                   |
   ------------------------------------------------
   | return-address [size=4]                      |
   ------------------------------------------------
   | caller-save registers [size=rsavesize]       |
   ------------------------------------------------
   | local variables [size=localsize]             |
   ------------------------------------------------
   | arguments to called functions [size=argsize] |
   ------------------------------------------------
   All sizes will be aligned as necessary.
   For a moving stack-pointer, the stack-pointer will usually point
   to the bottom of the area for local variables, but will move while
   arguments are put on the stack.

   This is just an example layout. Other layouts are also possible.
*/

static long real_offset(struct obj *o)
{
	long off = zm2l(o->v->offset);
//      printf("Parameter offset: %d, localsize: %d, rsavesize: %d\n",off,localsize,rsavesize);
	if (off < 0) {
		/* function parameter */
		off = localsize + rsavesize + 4 - off - zm2l(maxalign);
	}
	off += pushed;
	off += notyetpopped;
	off += zm2l(o->val.vmax);
	return off;
}


static int isstackparam(struct obj *o)
{
	int result=0;
//	if(o->flags&VAR && o->flags&REG && o->reg==sp)
//	if(o->flags&(VAR|DREFOBJ)==VAR)
	if(o->flags&VAR && !(o->flags&REG))
	{
		if(isauto(o->v->storage_class))
		{
			long off = zm2l(o->v->offset);
			if (off < 0)
				result=1;
		}
	}
	return(result);
}

/* Convenience function to determine whether we're assigning to 0(r6)
   and can thus use a more efficient writing sequence. */

int istopstackslot(struct obj *o)
{
	if(!o)
		return(0);
	if(o->v && (o->flags&(VAR|REG|DREFOBJ))==VAR)
	{
		if(isauto(o->v->storage_class)
				&& real_offset(o)==0)
			return(1);
	}
	return(0);
}

/* changes to a special section, used for __section() */
static int special_section(FILE * f, struct Var *v)
{
	char *sec;
	if (!v->vattr)
		return 0;
	sec = strstr(v->vattr, "section(");
	if (!sec)
		return 0;
	sec += strlen("section(");
	emit(f, "\t.section\t");
	while (*sec && *sec != ')')
		emit_char(f, *sec++);
	emit(f, "\n");
	if (f)
		section = SPECIAL;
	return 1;
}

/* Returns 1 if the symbol has weak linkage */
static int isweak(struct Var *v)
{
	if (!v->vattr)
		return 0;
	if (strstr(v->vattr, "weak"))
		return (1);
	return 0;
}

/* Emits a pointer to a function in a .ctor or .dtor section for automatic setup/cleanup */
static int ctor_dtor(FILE * f, struct Var *v)
{
	int dtor = 0;
	char *sec;
	if (!v->vattr)
		return 0;
	sec = strstr(v->vattr, "ctor(");
	if (!sec) {
		dtor = 1;
		sec = strstr(v->vattr, "dtor(");
	}
	if (!sec)
		return 0;
	sec += strlen("ctor(");
	emit(f, "\t%s.", dtor ? ".dtor .dtor" : ".ctor .ctor");
	while (*sec && *sec != ')')
		emit_char(f, *sec++);
	emit(f, "\n\t.ref\t%s%s\n", idprefix, v->identifier);

	return 1;
}


#define TEMP_TMP 0
#define TEMP_T1 1
struct tempobj
{
	struct obj o;
	int reg;
};
struct tempobj tempobjs[2];

void cleartempobj(FILE *f, int reg)
{
	int i;
	if(reg==tmp) i=TEMP_TMP;
	else if(reg==t1) i=TEMP_T1;
	else return;
//	emit(f,"// clearing %s\n",regnames[reg]);

	tempobjs[i].reg=0;
}

void settempkonst(FILE *f,int reg,int v)
{
	int i;
	if(reg==tmp) i=TEMP_TMP;
	else if(reg==t1) i=TEMP_T1;
	else return;
	tempobjs[i].reg=reg;
	tempobjs[i].o.flags=KONST;
	tempobjs[i].o.val.vlong=v;
//	emit(f,"// set %s to konst %d\n",regnames[reg],v);
}


// Add an adjustment due to postinc / predec to a cached object
void adjtempobj(FILE *f,int reg,int offset)
{
	if(reg<=1)
		tempobjs[reg].o.val.vlong+=offset;
}


// Store any passing value in tempobj records for optimisation.
// FIXME - need to figure out VARADR semantics for stored objects.
void settempobj(FILE *f,int reg,struct obj *o,int offset,int varadr)
{
	int i;
	if(reg==tmp) i=TEMP_TMP;
	else if(reg==t1) i=TEMP_T1;
	else return;
//	if(reg==t1)
//		emit(f,"// Setting %s to %x (%x)\n",regnames[reg],o,o->v);
	tempobjs[i].reg=reg;
	tempobjs[i].o=*o;
	tempobjs[i].o.val.vlong+=offset;	// Account for any postinc / predec
	if(varadr)
		tempobjs[i].o.flags|=VARADR;
}


// Compare a pair of struct obj* for equivalence.
// The first object should be the "live" object, the second one the cached object."
int matchobj(FILE *f,struct obj *o1,struct obj *o2,int varadr)
{
	int result=1;
	int flg=o1->flags;
	if(varadr)
		flg|=VARADR;
//	emit(f,"// comparing flags %x with %x\n",o1->flags, o2->flags);
//	if((o1->flags&~VARADR)!=(o2->flags&~VARADR))
	// FIXME - need to figure out VARADR semantics for stored objects.
	emit(f,"\t\t\t\t\t\t// matchobj comparing flags %d with %d\n",flg,o2->flags);
	if(flg!=(o2->flags))
		return(0);

//	emit(f,"// comparing regs %d with %d\n",o1->reg, o2->reg);
	// If the register-based value is being dereferenced we would have to track
	// the register itself being updated.  Unless the value's in tmp using a cached
	// version isn't a win anyway.
	if((o1->flags&(REG|DREFOBJ)==REG) && (o1->reg==o2->reg))
		return(1);

	if(o1->flags&KONST)
	{
//		emit(f,"\t\t\t\t\t\t// Comparing constants %x with %x\n",o1->val.vlong,o2->val.vlong);
		if(o1->val.vlong == o2->val.vlong)
			return(1);
		else
		{			// Attempt fuzzy matching...
			int d=o1->val.vlong-o2->val.vlong;
			// Don't bother if we need fewer than four LIs to represent the value, or if we'd need more than 1 LI for the offset.
			if(count_constantchunks(o1->val.vlong)<4 || count_constantchunks(d)>1)
			{
//				emit(f,"\t\t\t\t\t\t// Gains from fuzzy matching too small, ignoring.\n");
				return(0);
			}
			else
				return(2);
		}
	}

	if(!(o1->flags&VAR))
		return(0); // Not a var?  Can't do any more.

	if(o1->v==0 || o2->v==0)
		return(0);

	if(o1->v == o2->v && o1->val.vlong == o2->val.vlong)
		return(1);

	if(!(flg&VARADR))
		return(0); // Can only attempt fuzzy matching if this is a varadr

	if(isauto(o1->v->storage_class) && isauto(o2->v->storage_class))
	{
		if(DBGMSG)
			emit(f,"\t\t\t\t\t\t//auto: flags: %x, comparing %d, %d with %d, %d\n",
				flg,o1->v->offset,o1->val.vlong, o2->v->offset,o2->val.vlong);
		// Can't fuzzy match between parameters and vars on stack
		if((o1->v->offset<0 && o2->v->offset>=0) || (o1->v->offset>=0 && o2->v->offset<0))
			return(0);
		if(o1->v->offset==o2->v->offset && o1->val.vlong==o2->val.vlong)
			return(1);
		if((o1->flags&DREFOBJ) || (o2->flags&DREFOBJ))	// Can't fuzzy match if we're dereferencing.
			return(0);
		return(2);
	}

	if(isextern(o1->v->storage_class) && isextern(o2->v->storage_class))
	{
		if(DBGMSG)
			emit(f,"\t\t\t\t\t\t//extern: comparing %d with %d\n",o1->val.vlong, o2->val.vlong);
		if(strcmp(o1->v->identifier,o2->v->identifier))
			return(0);
		if(o1->val.vlong==o2->val.vlong)
			return(1);
		if((o1->flags&DREFOBJ) || (o2->flags&DREFOBJ))	// Can't fuzzy match if we're dereferencing.
			return(0);
		return(2);
	}

	return(0);
}


int matchoffset(struct obj *o,struct obj *o2)
{
	if(o->flags&KONST)
		return(o->val.vlong-o2->val.vlong);
	if(isextern(o->v->storage_class))
		return(o->val.vlong-o2->val.vlong);
	if(isauto(o->v->storage_class))
//		return((o->val.vlong+real_offset(o))-(o2->val.vlong+real_offset(o2)));
		return((o->val.vlong+o->v->offset)-(o2->val.vlong+o2->v->offset));
	return(0);
}


void obsoletetempobj(FILE *f,int reg,struct obj *o,int varadr)
{
//	emit(f,"\t\t\t\t\t// Attempting to obsolete obj\n");
	if(tempobjs[0].reg==reg && matchobj(f,o,&tempobjs[0].o,varadr))
	{
		emit(f,"\t\t\t\t\t\t// Obsoleting tmp\n");
		cleartempobj(f,tmp);
	}
	if(tempobjs[1].reg==reg && matchobj(f,o,&tempobjs[0].o,varadr))
	{
		emit(f,"\t\t\t\t\t\t// Obsoleting t1\n");
		cleartempobj(f,t1);
	}
}


// Check the tempobj records to see if the value we're interested in can be found in either.
int matchtempobj(FILE *f,struct obj *o,int varadr,int preferredreg)
{
	int hit=0;	// Hit will be 1 for an exact match, 2 for a near miss.
//	return(0); // Temporarily disable matching
	if(tempobjs[0].reg && (hit=matchobj(f,o,&tempobjs[0].o,varadr)))
	{
//		emit(f,"//match found - tmp\n");
//		printf("//match found - tmp\n");
		if(hit==1)
			return(tempobjs[0].reg);
		else if(hit==2)
		{
			int offset=matchoffset(o,&tempobjs[0].o);
			emit(f,"\t\t\t\t\t\t// Fuzzy match found against tmp.\n");
			if(preferredreg==tmp)
			{
				emit(f,"\tmr\t%s\n",regnames[t1]);
				emit(f,"\t.liconst\t%d\n",offset);
				emit(f,"\taddt\t%s\n",regnames[t1]);
				settempobj(f,t1,&tempobjs[0].o,0,0);
				settempobj(f,tmp,o,0,varadr);
			}
			else
			{
				emit(f,"\tmr\t%s\n",regnames[preferredreg]);
				emit(f,"\t.liconst\t%d\n",offset);
				settempkonst(f,tmp,offset);
				emit(f,"\tadd\t%s\n",regnames[preferredreg]);
				settempobj(f,preferredreg,o,0,varadr);
			}
			return(preferredreg);
//			return(tempobjs[0].reg);
		}
		else
			return(0);
	}
	else if(tempobjs[1].reg && (hit=matchobj(f,o,&tempobjs[1].o,varadr)))
	{
		// Temporarily disable t1 matching.  FIXME - keep t1 records more up-to-date.
//		return(0);
//		emit(f,"//match found - t1\n");
//		printf("//match found - t1\n");
		if(hit==1)
			return(tempobjs[1].reg);
		else if(hit==2)
		{
			int offset=matchoffset(o,&tempobjs[1].o);
			if(DBGMSG)
				emit(f,"\t\t\t\t\t\t//Fuzzy match found, offset: %d (varadr: %d)\n",offset,varadr);
			// Fuzzy match against t1 - if target is t1 use add, otherwise use addt.
			emit(f,"\t.liconst\t%d\n",offset);
			if(preferredreg!=tempobjs[1].reg)
			{
				emit(f,"\taddt\t%s\n",regnames[tempobjs[1].reg]);
				if(preferredreg!=tmp)
					emit(f,"\tmr\t%s\n",regnames[preferredreg]);
				settempobj(f,tmp,o,0,0);
				settempobj(f,preferredreg,o,0,varadr);
			}
			else
			{
				emit(f,"\tadd\t%s\n",regnames[tempobjs[1].reg]);
				settempkonst(f,tmp,offset);
				settempobj(f,tempobjs[1].reg,o,0,varadr);
			}
			return(preferredreg);
		}
		return(0);
	}
	else
		return(0);
}


int matchtempkonst(FILE *f,int k,int preferredreg)
{
//	return(0); // Temporarily disable matching
	struct obj o;
	o.flags=KONST;
	o.val.vlong=k;
	return(matchtempobj(f,&o,0,preferredreg));
}


/*  Generates code to store register r into memory object o. */

static void store_reg(FILE * f, int r, struct obj *o, int type)
{
	// Need to take different types into account here.
	if(DBGMSG)
		emit(f, "\t\t\t\t\t\t// Store_reg to type 0x%x, flags 0x%x\n", type,o->flags);

	type &= NQ;		// Filter out unsigned, etc.
	if((type==CHAR || type==SHORT) && isstackparam(o))
	{
		emit(f, "\t\t\t\t\t\t// Promoting storage size of stack parameter to int\n");
		type=INT;
	}

	switch (type) {
	case CHAR:
		emit_prepobj(f, o, type & NQ, tmp, 0);
		emit(f, "\texg\t%s\n", regnames[r]);
		emit(f, "\tstbinc\t%s\t//WARNING - pointer / reg not restored, might cause trouble!\n", regnames[r]);
		cleartempobj(f,tmp);
		cleartempobj(f,r);
		break;
	case SHORT:
		emit_prepobj(f, o, type & NQ, tmp, 0);
		emit(f, "\texg\t%s\n", regnames[r]);
		emit(f, "\thlf\n\tst\t%s\n", regnames[r]);
		cleartempobj(f,tmp);
		cleartempobj(f,r);
		break;
	case INT:
	case LONG:
	case POINTER:
		// if o is a reg, can store directly.
		if ((o->flags & (REG | DREFOBJ)) == (REG | DREFOBJ)) {
			emit(f, "\tmt\t%s\n", regnames[r]);
			emit(f, "\tst\t%s\n", regnames[o->reg]);
//			settempobj(f,r,o,0,0);
			if((type&NQ)!=INT || (type & VOLATILE) || (type & PVOLATILE))
			{
				emit(f,"\t// Volatile, or not int - not caching\n");
				cleartempobj(f,r);
			}
			else
				settempobj(f,r,o,0,0); // FIXME - is this correct?
			settempobj(f,tmp,o,0,0); // FIXME - is this correct?
		} else {
			if(o->flags & DREFOBJ) {  // Can't use the offset / stmpdec trick for dereferenced objects.
				// FIXME, not strictly true - could use it for dereferenced constants
				emit_prepobj(f, o, type & NQ, tmp, 0);
				emit(f, "\texg\t%s\n", regnames[r]);
				emit(f, "\tst\t%s\n", regnames[r]);
				if(r==t1 || (o->am && o->am->disposable))
					emit(f, "\t\t\t\t\t\t// WARNING - Object is disposable, not bothering to undo exg - check correctness\n");
				else
					emit(f, "\texg\t%s\n", regnames[r]);
				cleartempobj(f,tmp);
				cleartempobj(f,r);
			}
			else {
				emit_prepobj(f, o, type & NQ, tmp, 4);	// stmpdec predecrements, so need to add 4!
				emit(f, "\tstmpdec\t%s\n \t\t\t\t\t\t// WARNING - check that 4 has been added.\n", regnames[r]);
				adjtempobj(f,tmp,-4);
//				cleartempobj(f,tmp);
				if((type&NQ)!=INT || (type & VOLATILE) || (type & PVOLATILE))
				{
					emit(f,"\t// Volatile, or not int - not caching\n");
					cleartempobj(f,r);
				}
				else
					settempobj(f,r,o,0,0); // FIXME - is this correct?
			}
		}
		break;
	case LLONG:
		if ((o->flags & (REG | DREFOBJ)) == (REG | DREFOBJ)) {
			emit_prepobj(f, o, type & NQ, tmp, 0);
			printf("store_reg: storing long long to dereferenced register\n");
			emit(f,"//FIXME - need to store 64-bits\n");
			ierror(0);
		}
		else {
			// 
			printf("store_reg: storing long long in %s to reg\n",regnames[r]);
			ierror(0);
		}
		break;		
	default:
		printf("store_reg: unhandled type 0x%x\n", type);
		ierror(0);
		break;
	}
}


/*  Yields log2(x)+1 or 0. */
static long pof2(zumax x)
{
	zumax p;
	int ln = 1;
	p = ul2zum(1L);
	while (ln <= 32 && zumleq(p, x)) {
		if (zumeqto(x, p))
			return ln;
		ln++;
		p = zumadd(p, p);
	}
	return 0;
}


static int availreg()
{
	int i;
	for(i=FIRST_GPR+RESERVED_GPRS;i<(LAST_GPR-1);++i)
		if(regs[i]==0)
			return(i);
	return(0);
}


static struct IC *preload(FILE *, struct IC *,int stacksubst);

static void function_top(FILE *, struct Var *, long);
static int function_bottom(FILE * f, struct Var *, long,int);

#define isreg(x) ((p->x.flags&(REG|DREFOBJ))==REG)
#define involvesreg(x) ((p->x.flags&(REG))==REG)
#define isconst(x) ((p->x.flags&(KONST|DREFOBJ))==KONST)

static int q1reg, q2reg, zreg;

static char *ccs[] = { "EQ", "NEQ", "SLT", "GE", "LE", "SGT", "EX", "" };
static char *logicals[] = { "or", "xor", "and" };

static char *arithmetics[] = { "shl", "shr", "add", "sub", "mul", "(div)", "(mod)" };

/* Does some pre-processing like fetching operands from memory to
   registers etc. */
static struct IC *preload(FILE * f, struct IC *p,int stacksubst)
{
	int r;

	if(stacksubst)
	{
		if(istopstackslot(&p->q1))
		{
			p->q1.reg=sp;
			p->q1.flags|=REG|DREFOBJ;
		}

		if(istopstackslot(&p->q2))
		{
			p->q2.reg=sp;
			p->q2.flags|=REG|DREFOBJ;
		}

		if(istopstackslot(&p->z))
		{
			p->z.reg=sp;
			p->z.flags|=REG|DREFOBJ;
		}
	}

	if (involvesreg(q1))
		q1reg = p->q1.reg;
	else
		q1reg = 0;

	if (involvesreg(q2))
		q2reg = p->q2.reg;
	else
		q2reg = 0;

	if (isreg(z)) {
		zreg = p->z.reg;
	} else {
		if (ISFLOAT(ztyp(p)))
			zreg = f1;
		else
			zreg = t1;
	}

	return p;
}

/* Determine whether the register we're about to write to will merely be passed to SetReturn.
   If so, return 1, and convert the SetReturn IC to NOP */
int next_setreturn(struct IC *p,int reg)
{
	int result=0;
	struct IC *p2=p->next;
	while(p2 && p2->code==FREEREG)
		p2=p2->next;
	if(p2 && p2->code==SETRETURN && (p2->q1.flags&(REG|DREFOBJ))==REG && p2->q1.reg==reg)
	{
		p2->code=NOP;
		result=1;
	}
	return(result);
}


int consecutiveaccess(struct IC *p,struct IC *p2)
{
	if(!p || !p2)
		return(0);
//	printf("Flags %x, %x\n",p->z.flags,p2->z.flags);
	if(((p->z.flags&(VAR|DREFOBJ))==VAR) && ((p2->z.flags&(VAR|DREFOBJ))==VAR))
	{
		int result=real_offset(&p2->z)-real_offset(&p->z);
//		printf("Got two vars\n");
		if(strcmp(p->z.v->identifier,p2->z.v->identifier))
			return(0);
		if(isstatic(p->z.v->storage_class) && isstatic(p->z.v->storage_class))
		{
//			printf("Both static - dif %d\n",result);
			return(result);
		}
		if(isextern(p->z.v->storage_class) && isextern(p->z.v->storage_class))
		{
//			printf("Both extern - dif %d\n",result);
			return(result);
		}
	}
	return(0);
}

/* save the result (in temp) into p->z */
/* Guaranteed not to touch t1/t2 unless nominated */
/* or followed by a SetReturn IC. */
void save_temp(FILE * f, struct IC *p, int treg)
{
	int type = ztyp(p) & NQ;
	if(DBGMSG)
		emit(f, "\t\t\t\t\t\t// (save temp)");

	if (isreg(z)) {
		int target=p->z.reg;
		if(DBGMSG)
			emit(f, "isreg\n");
		if(next_setreturn(p,target))
			target=t1;
		emit(f, "\tmr\t%s\n", regnames[target]);
	} else {
		if ((p->z.flags & DREFOBJ) && (p->z.flags & REG))
			treg = p->z.reg;
		else if(isstackparam(&p->z))
			type=INT;

		if(DBGMSG)
			emit(f, "store type %x\n",type);

		switch (type) {
		case CHAR:
			if (p->z.am && p->z.am->type == AM_POSTINC)
			{
				emit(f, "\tstbinc\t%s\n", regnames[treg]);
				adjtempobj(f,treg,1);
			}
			else if ((p->z.am && p->z.am->disposable)
				 || (treg == t1))
			{
				emit(f, "\tstbinc\t%s\n\t\t\t\t\t\t//Disposable, postinc doesn't matter.\n", regnames[treg]);
				adjtempobj(f,treg,1);
			}
			else
				emit(f, "\tbyt\n\tst\t%s\n", regnames[treg]);
			break;
		case SHORT:
			emit(f, "\thlf\n\tst\t%s\n", regnames[treg]);
			break;
		case INT:
		case LONG:
		case POINTER:
//			// Would need to adjust the pointer at the setup stage since we're predecrementing
//			if (consecutiveaccess(p,p->next)==-4 || (p->z.am && p->z.am->type == AM_PREDEC))
//			{
//				emit(f, "\tstdec\t%s\n", regnames[treg]);
//				adjtempobj(f,treg,-4);
//			}
			if (consecutiveaccess(p,p->next)==4 || (p->z.am && p->z.am->type == AM_POSTINC))
			{
				emit(f, "\tstinc\t%s\n", regnames[treg]);
				adjtempobj(f,treg,4);
			}
			else
				emit(f, "\tst\t%s\n", regnames[treg]);
			break;
		default:
			printf("save_temp - type %d not yet handled\n", ztyp(p));
			emit(f,"\t\t\t\t\t\t// FIXME - save_temp doesn't support size\n");
			break;
		}
	}
	if(DBGMSG)
		emit(f, "\t\t\t\t\t\t//save_temp done\n");
}

/* save the result (in zreg) into p->z */
void save_result(FILE * f, struct IC *p)
{
	if(DBGMSG)
		emit(f, "\t\t\t\t\t\t// (save result) ");
	if (isreg(z)) {
		if(DBGMSG)
			emit(f, "// isreg\n");
		if (p->z.reg != zreg)
		{
			emit(f, "\tmt\t%s\n\tmr\t%s\n", regnames[zreg], regnames[p->z.reg]);
			settempobj(f,tmp,&p->z,0,0);
			settempobj(f,p->z.reg,&p->z,0,0);
		}
	}
	else
	{
		emit(f, "// not reg\n");
		store_reg(f,zreg,&p->z,ztyp(p));
	}
	return;
}

#include "addressingmodes.c"
#include "tempregs.c"
#include "inlinememcpy.c"
#include "libcalls.c"

/* generates the function entry code */
static void function_top(FILE * f, struct Var *v, long offset)
{
	int i;
	int regcount = 0;

	cleartempobj(f,tmp);
	cleartempobj(f,t1);

	if(DBGMSG)
	{
		emit(f, "\t//registers used:\n");
		for (i = FIRST_GPR+RESERVED_GPRS; i <= LAST_GPR; ++i) {
			emit(f, "\t\t//%s: %s\n", regnames[i], regused[i] ? "yes" : "no");
			if (regused[i] && (i >= (FIRST_GPR+SCRATCH_GPRS+RESERVED_GPRS)) && (i <= LAST_GPR - 2))
				++regcount;
		}
	}

// Emit ctor / dtor tables
	ctor_dtor(f, v);

	rsavesize = 0;
	if (!special_section(f, v)) {
		emit(f, "\t.section\t.text.%x\n", sectionid);
		section=CODE;
		++sectionid;
	}
	if (v->storage_class == EXTERN) {
		if ((v->flags & (INLINEFUNC | INLINEEXT)) != INLINEFUNC) {
			if (isweak(v))
				emit(f, "\t.weak\t%s%s\n", idprefix, v->identifier);
			else
				emit(f, "\t.global\t%s%s\n", idprefix, v->identifier);
		}
		emit(f, "%s%s:\n", idprefix, v->identifier);
	} else
		emit(f, "%s%ld:\n", labprefix, zm2l(v->offset));

	if (regcount < 3) {
		emit(f, "\tstdec\t%s\n", regnames[sp]);
		for (i = FIRST_GPR + SCRATCH_GPRS; i <= LAST_GPR - 3; ++i) {
			if (regused[i] && !regscratch[i]) {
				emit(f, "\tmt\t%s\n\tstdec\t%s\n", regnames[i], regnames[sp]);
				rsavesize += 4;
			}
		}
	} else {
		emit(f, "\texg\t%s\n\tstmpdec\t%s\n", regnames[sp], regnames[sp]);
		for (i = FIRST_GPR + SCRATCH_GPRS; i <= LAST_GPR - 3; ++i) {
			if (regused[i] && !regscratch[i]) {
				emit(f, "\tstmpdec\t%s\n", regnames[i]);
				rsavesize += 4;
			}
		}
		emit(f, "\texg\t%s\n", regnames[sp]);
	}

	// FIXME - Allow the stack to float, in the hope that we can use stdec to adjust it.

	if ((offset == 4) && optsize)
		emit(f, "\tstdec\tr6\t// shortest way to decrement sp by 4\n");
	else if (offset) {
		emit_constanttotemp(f, -offset);
		emit(f, "\tadd\t%s\n", regnames[sp]);
	}
}

/* generates the function exit code */
/* Returns 1 if tail code was generated. */
static int function_bottom(FILE * f, struct Var *v, long offset,int firsttail)
{
	int i;
	int tail=0;

	int regcount = 0;
	for (i = FIRST_GPR + SCRATCH_GPRS + RESERVED_GPRS; i <= LAST_GPR - 3; ++i) {
		if (regused[i] && !regscratch[i])
			++regcount;
	}

	if ((offset == 4) && optsize)
		emit(f, "\tldinc\t%s\t// shortest way to add 4 to sp\n", regnames[sp]);
	else if (offset) {
		emit_constanttotemp(f, -offset);	// Negative range extends one integer further than positive range.
		emit(f, "\tsub\t%s\n", regnames[sp]);
	}

	if(optsize) // If we're optimising for size we can potentially save some bytes in the function tails.
	{
		if(regcount)
		{
			/* We have to restore some registers.  Jump into the tail code at the appropriate place. */
			if(regcount<(5-SCRATCH_GPRS) || !firsttail)
			{
				emit(f,"\t.lipcrel\t.functiontail, %d\n",((5-SCRATCH_GPRS)-regcount)*2);
				emit(f,"\tadd\t%s\n\n",regnames[pc]);
			}
			if(firsttail)
			{
				/* This is the first time we've needed to restore registers - generate tail code */
				emit(f,".functiontail:\n");
				for (i = LAST_GPR - 3; i >= FIRST_GPR + SCRATCH_GPRS; --i) {
					if (!regscratch[i])
						emit(f, "\tldinc\t%s\n\tmr\t%s\n\n", regnames[sp], regnames[i]);
				}
				emit(f, "\tldinc\t%s\n\tmr\t%s\n\n", regnames[sp], regnames[pc]);
				if(f)
					tail=1; /* Higher optimisation levels do a dummy run with null file */
			}
		}
		else
		{
			/* Didn't need to preserve any registers, just restore PC */
			emit(f, "\tldinc\t%s\n\tmr\t%s\n\n", regnames[sp], regnames[pc]);
		}
	}
	else
	{
		for (i = LAST_GPR - 3; i >= FIRST_GPR + SCRATCH_GPRS; --i) {
			if (regused[i] && !regscratch[i])
				emit(f, "\tldinc\t%s\n\tmr\t%s\n\n", regnames[sp], regnames[i]);
		}
		emit(f, "\tldinc\t%s\n\tmr\t%s\n\n", regnames[sp], regnames[pc]);
	}
	return(tail);
}

/****************************************/
/*  End of private data and functions.  */
/****************************************/

/*  Does necessary initializations for the code-generator. Gets called  */
/*  once at the beginning and should return 0 in case of problems.      */
int init_cg(void)
{
	int i;
	/*  Initialize some values which cannot be statically initialized   */
	/*  because they are stored in the target's arithmetic.             */
	maxalign = l2zm(4L);
	char_bit = l2zm(8L);
	stackalign = l2zm(4);

	// We have full load-store align, so in size mode we can pack data more tightly...

	for (i = 0; i <= MAX_TYPE; i++) {
		sizetab[i] = l2zm(msizetab[i]);
		align[i] = optsize ? 1 : l2zm(malign[i]);

// Can't align everything to 4 bytes for speed without messing up struct packing.  Is there a better way?
//		align[i] = optspeed ? 4 : (optsize ? 1 : l2zm(malign[i]));
//		align[i] = l2zm(malign[i]);
	}

	regnames[0] = "noreg";
	for (i = FIRST_GPR; i <= LAST_GPR - 1; i++) {
		regnames[i] = mymalloc(5);
		sprintf(regnames[i], "r%d", i - FIRST_GPR);
		regsize[i] = l2zm(4L);
		regtype[i] = &ltyp;
		regsa[i] = 0;
	}
	regnames[i] = mymalloc(5);
	sprintf(regnames[i], "tmp");
	regsize[i] = l2zm(4L);
	regtype[i] = &ltyp;
	regsa[i] = 1;
	for (i = FIRST_FPR; i <= LAST_FPR; i++) {
		regnames[i] = mymalloc(10);
		sprintf(regnames[i], "fpr%d", i - FIRST_FPR);
		regsize[i] = l2zm(8L);
		regtype[i] = &ldbl;
	}

	/*  Use multiple ccs.   */
	multiple_ccs = 0;

	/*  Initialize the min/max-settings. Note that the types of the     */
	/*  host system may be different from the target system and you may */
	/*  only use the smallest maximum values ANSI guarantees if you     */
	/*  want to be portable.                                            */
	/*  That's the reason for the subtraction in t_min[INT]. Long could */
	/*  be unable to represent -2147483648 on the host system.          */
	t_min[CHAR] = l2zm(-128L);
	t_min[SHORT] = l2zm(-32768L);
	t_min[INT] = zmsub(l2zm(-2147483647L), l2zm(1L));
	t_min[LONG] = t_min(INT);
	t_min[LLONG] = zmlshift(l2zm(1L), l2zm(63L));
	t_min[MAXINT] = t_min(LLONG);
	t_max[CHAR] = ul2zum(127L);
	t_max[SHORT] = ul2zum(32767UL);
	t_max[INT] = ul2zum(2147483647UL);
	t_max[LONG] = t_max(INT);
	t_max[LLONG] = zumrshift(zumkompl(ul2zum(0UL)), ul2zum(1UL));
	t_max[MAXINT] = t_max(LLONG);
	tu_max[CHAR] = ul2zum(255UL);
	tu_max[SHORT] = ul2zum(65535UL);
	tu_max[INT] = ul2zum(4294967295UL);
	tu_max[LONG] = t_max(UNSIGNED | INT);
	tu_max[LLONG] = zumkompl(ul2zum(0UL));
	tu_max[MAXINT] = t_max(UNSIGNED | LLONG);

	/*  Reserve a few registers for use by the code-generator.      */
	/*  This is not optimal but simple.                             */
	tmp = FIRST_GPR + 8;
	pc = FIRST_GPR + 7;
	sp = FIRST_GPR + 6;
	t1 = FIRST_GPR;		// r0, also return register.
	t2 = FIRST_GPR + 1;
//  f1=FIRST_FPR;
//  f2=FIRST_FPR+1;

	for (i = FIRST_GPR; i <= LAST_GPR; i++)
		regscratch[i] = 0;
	for (i = FIRST_FPR; i <= LAST_FPR; i++)
		regscratch[i] = 0;

	regsa[FIRST_GPR] = 1;	// Allocate the return register
	regsa[t1] = 1;
	regsa[t2] = 0;
	regsa[sp] = 1;
	regsa[pc] = 1;
	regsa[tmp] = 1;
	regscratch[FIRST_GPR] = 0;
	for(i=FIRST_GPR+RESERVED_GPRS;i<=(FIRST_GPR+SCRATCH_GPRS);++i)
		regscratch[i] = 1;
	regscratch[sp] = 0;
	regscratch[pc] = 0;

	target_macros = marray;

	return 1;
}

void init_db(FILE * f)
{
}

int freturn(struct Typ *t)
/*  Returns the register in which variables of type t are returned. */
/*  If the value cannot be returned in a register returns 0.        */
/*  A pointer MUST be returned in a register. The code-generator    */
/*  has to simulate a pseudo register if necessary.                 */
{
	if (ISFLOAT(t->flags))
		return 0;
	if (ISSTRUCT(t->flags) || ISUNION(t->flags))
		return 0;
	if (zmleq(szof(t), l2zm(4L)))
		return FIRST_GPR;
	else
		return 0;
}

int reg_pair(int r, struct rpair *p)
/* Returns 0 if the register is no register pair. If r  */
/* is a register pair non-zero will be returned and the */
/* structure pointed to p will be filled with the two   */
/* elements.                                            */
{
	return 0;
}

/* estimate the cost-saving if object o from IC p is placed in
   register r */
int cost_savings(struct IC *p, int r, struct obj *o)
{
	int c = p->code;
	if(o->v && isextern(o->v->storage_class))  // Externs are particularly costly due to the ldinc r7 shuffle
		return(o->flags & DREFOBJ ? 5 : 3);
	if (o->flags & VKONST) {
		if (isextern(o->flags) || isstatic(o->flags))
			return 2;
		else {
			struct obj *o2 = &o->v->cobj;
			int c = count_constantchunks(o2->val.vmax);
			return c - 1;
		}
	}
	if (o->flags & DREFOBJ)
		return 2;
	if (c == SETRETURN)// && r == p->z.reg && !(o->flags & DREFOBJ))
		return 1;
	if (c == GETRETURN)// && r == p->q1.reg && !(o->flags & DREFOBJ))
		return 1;
	return 1;
}

int regok(int r, int t, int mode)
/*  Returns 0 if register r cannot store variables of   */
/*  type t. If t==POINTER and mode!=0 then it returns   */
/*  non-zero only if the register can store a pointer   */
/*  and dereference a pointer to mode.                  */
{
	if (r == 0)
		return 0;
	t &= NQ;
	if (ISFLOAT(t) && r >= FIRST_FPR && r <= LAST_FPR)
		return 1;
	if (t == POINTER && r >= FIRST_GPR && r <= LAST_GPR)
		return 1;
	if (t >= CHAR && t <= LONG && r >= FIRST_GPR && r <= LAST_GPR)
		return 1;
	return 0;
}

int dangerous_IC(struct IC *p)
/*  Returns zero if the IC p can be safely executed     */
/*  without danger of exceptions or similar things.     */
/*  vbcc may generate code in which non-dangerous ICs   */
/*  are sometimes executed although control-flow may    */
/*  never reach them (mainly when moving computations   */
/*  out of loops).                                      */
/*  Typical ICs that generate exceptions on some        */
/*  machines are:                                       */
/*      - accesses via pointers                         */
/*      - division/modulo                               */
/*      - overflow on signed integer/floats             */
{
	int c = p->code;
	if ((p->q1.flags & DREFOBJ) || (p->q2.flags & DREFOBJ)
	    || (p->z.flags & DREFOBJ))
		return 1;
	if ((c == DIV || c == MOD) && !isconst(q2))
		return 1;
	return 0;
}

int must_convert(int o, int t, int const_expr)
/*  Returns zero if code for converting np to type t    */
/*  can be omitted.                                     */
/*  On the PowerPC cpu pointers and 32bit               */
/*  integers have the same representation and can use   */
/*  the same registers.                                 */
{
	int op = o & NQ, tp = t & NQ;
	if ((op == INT || op == LONG || op == POINTER)
	    && (tp == INT || tp == LONG || tp == POINTER))
		return 0;
	if (op == DOUBLE && tp == LDOUBLE)
		return 0;
	if (op == LDOUBLE && tp == DOUBLE)
		return 0;
	return 1;
}

void gen_ds(FILE * f, zmax size, struct Typ *t)
/*  This function has to create <size> bytes of storage */
/*  initialized with zero.                              */
{
	if (newobj && section != SPECIAL)
		emit(f, "%ld\n", zm2l(size));
	else
		emit(f, "\t.space\t%ld\n", zm2l(size));
	newobj = 0;
}


/*  This function has to make sure the next data is
    aligned to multiples of <align> bytes.
    If the speed optimisation flag is set, always align
    to four bytes. */
void gen_align(FILE * f, zmax align)
{
	if(optspeed)
		emit(f,"\t.align\t4\n");
	else if (zm2l(align) > 1)
		emit(f, "\t.align\t%d\n", align);
}

void gen_var_head(FILE * f, struct Var *v)
/*  This function has to create the head of a variable  */
/*  definition, i.e. the label and information for      */
/*  linkage etc.                                        */
{
	int constflag;
	char *sec;
	if (v->clist)
		constflag = is_const(v->vtyp);
	if (v->storage_class == STATIC) {
		if (ISFUNC(v->vtyp->flags))
			return;
		if (!special_section(f, v)) {
			if (v->clist && (!constflag)) { // || (g_flags[2] & USEDFLAG))
//			    && section != DATA) {
				emit(f, "%s.%x\n",dataname,sectionid);
				++sectionid;
				if (f)
					section = DATA;
			}
			if (v->clist && constflag) { // && !(g_flags[2] & USEDFLAG)
//			    && section != RODATA) {
				emit(f, "%s.%x\n",rodataname,sectionid);
				++sectionid;
				if (f)
					section = RODATA;
			}
			if (!v->clist) { // && section != BSS) {
				emit(f, "%s.%x\n",bssname,sectionid);
				++sectionid;
				if (f)
					section = BSS;
			}
		}
		if (v->clist || section == SPECIAL) {
			gen_align(f, falign(v->vtyp));
			emit(f, "%s%ld:\n", labprefix, zm2l(v->offset));
		} else {
			gen_align(f, falign(v->vtyp));
			emit(f, "\t.lcomm\t%s%ld,", labprefix, zm2l(v->offset));
		}
		newobj = 1;
	}
	if (v->storage_class == EXTERN) {
//		emit(f, "\t.global\t%s%s\n", idprefix, v->identifier);
		if (v->flags & (DEFINED | TENTATIVE)) {
			if (!special_section(f, v)) {
				if (v->clist && (!constflag)) { // || (g_flags[2] & USEDFLAG))
//				    && section != DATA) {
					emit(f, "%s.%x\n",dataname,sectionid);
					++sectionid;
					if (f)
						section = DATA;
				}
				if (v->clist && constflag) { // && !(g_flags[2] & USEDFLAG)
//				    && section != RODATA) {
					emit(f, "%s.%x\n",rodataname,sectionid);
					++sectionid;
					if (f)
						section = RODATA;
				}
				if (!v->clist) { // && section != BSS) {
					emit(f, "%s.%x\n",bssname,sectionid);
					++sectionid;
					if (f)
						section = BSS;
				}
			}
			if (v->clist || section == SPECIAL) {
				gen_align(f, falign(v->vtyp));
				if (isweak(v))
					emit(f, "\t.weak\t%s%s\n", idprefix, v->identifier);
				else
					emit(f, "\t.global\t%s%s\n", idprefix, v->identifier);
				emit(f, "%s%s:\n", idprefix, v->identifier);
			} else {
				gen_align(f, falign(v->vtyp));
				if (isweak(v))
					emit(f, "\t.weak\t%s%s\n", idprefix, v->identifier);
				else {
					emit(f, "\t.global\t%s%s\n", idprefix, v->identifier);
					emit(f, "\t.comm\t%s%s,", idprefix, v->identifier);
				}
			}
			newobj = 1;
		}
	}
}

void gen_dc(FILE * f, int t, struct const_list *p)
/*  This function has to create static storage          */
/*  initialized with const-list p.                      */
{
	if (!p->tree) {
		switch (t & NQ) {
		case CHAR:
			emit(f, "\t.byte\t");
			break;
		case SHORT:
			emit(f, "\t.short\t");
			break;
		case LONG:
		case INT:
		case MAXINT:
		case POINTER:
			emit(f, "\t.int\t");
			break;
		case LLONG:
			emit(f, "//FIXME - unsupported type\n");
			emit(f, "\t.long\t");
//			ierror(0);
			break;
		default:
			printf("gen_dc: unsupported type 0x%x\n", t);
			ierror(0);
		}
		emitval(f, &p->val, t & NU);
		emit(f, "\n");

#if 0
		if (ISFLOAT(t)) {
			/*  auch wieder nicht sehr schoen und IEEE noetig   */
			unsigned char *ip;
			ip = (unsigned char *)&p->val.vdouble;
			emit(f, "0x%02x%02x%02x%02x", ip[0], ip[1], ip[2], ip[3]);
			if ((t & NQ) != FLOAT) {
				emit(f, ",0x%02x%02x%02x%02x", ip[4], ip[5], ip[6], ip[7]);
			}
		} else {
			emitval(f, &p->val, t & NU);
		}
#endif
	} else {
		struct obj *o = &p->tree->o;
		emit(f, "\t\t\t\t\t\t// Declaring from tree\n");
		if (isextern(o->v->storage_class)) {
			emit(f, "\t\t\t\t\t\t// extern (offset %d)\n", o->val.vmax);
			if (o->val.vmax)
				emit(f, "\t.ref\t_%s, %d\n", o->v->identifier, o->val.vmax);
			else
				emit(f, "\t.ref\t_%s\n", o->v->identifier);
		} else if (isstatic(o->v->storage_class)) {
			emit(f, "\t\t\t\t\t\t// static\n");
			if(o->val.vlong)
				emit(f, "\t.ref\t%s%d,%d\n", labprefix, zm2l(o->v->offset),o->val.vlong);
			else
				emit(f, "\t.ref\t%s%d\n", labprefix, zm2l(o->v->offset));
		} else {
			printf("error: GenDC (tree) - unknown storage class 0x%x!\n", o->v->storage_class);
		}
	}
	newobj = 0;
}


/* Return 1 if any of p's operands uses predec or postinc addressing mode */
int check_am(struct IC *p)
{
	if(p->q1.am && (p->q1.am->type==AM_POSTINC || p->q1.am->type==AM_PREDEC))
		return(1);
	if(p->q2.am && (p->q2.am->type==AM_POSTINC || p->q2.am->type==AM_PREDEC))
		return(1);
	if(p->z.am && (p->z.am->type==AM_POSTINC || p->z.am->type==AM_PREDEC))
		return(1);
	return(0);
}

/*  The main code-generation routine.                   */
/*  f is the stream the code should be written to.      */
/*  p is a pointer to a doubly linked list of ICs       */
/*  containing the function body to generate code for.  */
/*  v is a pointer to the function.                     */
/*  offset is the size of the stackframe the function   */
/*  needs for local variables.                          */

void gen_code(FILE * f, struct IC *p, struct Var *v, zmax offset)
/*  The main code-generation.                                           */
{
	static int idemp = 0;
	static int firsttail=1;
	int reversecmp=0;
	int c, t, i;
	struct IC *m;
	argsize = 0;
	// if(DEBUG&1) 

	if(!p)
		printf("(gen_code called with null IC list?)\n");

	for (c = 1; c <= MAXR; c++)
		regs[c] = regsa[c];
	pushed = 0;
	notyetpopped = 0;

#if 0
	if (!idemp) {
		sectionid = 0;
		if (p && p->file) {
			int v;
			char *c = p->file;
			idemp = 1;
			while (v = *c++) {
				sectionid <<= 3;
				sectionid ^= v;
			}
			printf("Created section ID %x\n",sectionid);
		}
		else
			printf("No sectionid created (%x, %x)\n",p,p ? p->file : 0);
	}
#endif

	for (m = p; m; m = m->next) {
		c = m->code;
		t = m->typf & NU;
		if (c == ALLOCREG) {
			regs[m->q1.reg] = 1;
			continue;
		}
		if (c == FREEREG) {
			regs[m->q1.reg] = 0;
			continue;
		}

		/* convert MULT/DIV/MOD with powers of two */
		// Perversely, mul is faster than shifting on 832, so we only want to do this for div.
		// FIXME - we can do this for signed values too.
		if ((t & NQ) <= LONG && (m->q2.flags & (KONST | DREFOBJ)) == KONST && (t & NQ) <= LONG
		    && (((c == DIV || c == MOD) && (t & UNSIGNED)))) {
			eval_const(&m->q2.val, t);
			i = pof2(vmax);
			if (i) {
				if (c == MOD) {
					vmax = zmsub(vmax, l2zm(1L));
					m->code = AND;
				} else {
					vmax = l2zm(i - 1);
					if (c == DIV)
						m->code = RSHIFT;
					else
						m->code = LSHIFT;
				}
				c = m->code;
				gval.vmax = vmax;
				eval_const(&gval, MAXINT);
				if (c == AND) {	// FIXME - why?
					insert_const(&m->q2.val, t);
				} else {
					insert_const(&m->q2.val, INT);
					p->typf2 = INT;
				}
			}
		}
	}

	for (c = 1; c <= MAXR; c++) {
		if (regsa[c] || regused[c]) {
			BSET(regs_modified, c);
		}
	}
	localsize = (zm2l(offset) + 3) / 4 * 4;

//    printf("\nSeeking addressing modes for function %s\n",v->identifier);
	find_addressingmodes(p);

	function_top(f, v, localsize);
//    printf("%s:\n",v->identifier);

	for (; p; p = p->next) {
//		printic(stdout,p);
		c = p->code;
		t = q1typ(p);

		if (c == NOP) {
			p->z.flags = 0;
			continue;
		}
		if (c == ALLOCREG) {
			if(DBGMSG)
				emit(f, "\t\t\t\t\t\t// allocreg %s\n", regnames[p->q1.reg]);
			regs[p->q1.reg] = 1;
			continue;
		}
		if (c == FREEREG) {
			if(DBGMSG)
				emit(f, "\t\t\t\t\t\t// freereg %s\n", regnames[p->q1.reg]);
			regs[p->q1.reg] = 0;
			continue;
		}
		if (c == LABEL) {
			int i;
			emit(f, "%s%d: # \n", labprefix, t);
			cleartempobj(f,tmp);	// Can't carry temporary context past a label
			cleartempobj(f,t1);
			continue;
		}

		if (DBGMSG && p->file)
			emit(f, "\n\t\t\t\t\t\t//%s, line %d\n", p->file, p->line);
		if(p->q1.am && p->q1.am->disposable)
			emit(f, "\t\t\t\t\t\t// Q1 disposable\n");
		if(p->q2.am && p->q2.am->disposable)
			emit(f, "\t\t\t\t\t\t// Q2 disposable\n");
		if(p->z.am && p->z.am->disposable)
			emit(f, "\t\t\t\t\t\t// Z disposable\n");

		// OK
		if (c == BRA) {
			if(0) // FIXME - could duplicate function tail here.  Perhaps do it depending on number of saved registers?
				function_bottom(f, v, localsize, 0);
			else
				emit_pcreltotemp(f, labprefix, t);
			emit(f, "\tadd\t%s\n", regnames[pc]);
			continue;
		}
		// OK
		if (c >= BEQ && c < BRA) {
			if(reversecmp)
			{
				switch(c)
				{
					case BLT:
						c=BGT;
						break;
					case BLE:
						c=BGE;
						break;
					case BGT:
						c=BLT;
						break;
					case BGE:
						c=BLE;
						break;
				}
			}
			emit(f, "\tcond\t%s\n",ccs[c - BEQ]);
			if(DBGMSG)
				emit(f, "\t\t\t\t\t\t//conditional branch %s\n",reversecmp ? "reversed" : "regular");
			reversecmp=0;
			emit_pcreltotemp(f, labprefix, t);	// FIXME - double-check that we shouldn't include an offset here.
			emit(f, "\t\tadd\tr7\n");
			continue;
		}
		// Investigate - but not currently seeing it used.
		if (c == MOVETOREG) {
			emit(f, "\t\t\t\t\t\t//CHECKME movetoreg\n");
			emit_objtoreg(f, &p->q1, ztyp(p),zreg);
			continue;
		}
		// Investigate - but not currently seeing it used.
		if (c == MOVEFROMREG) {
			emit(f, "\t\t\t\t\t\t//CHECKME movefromreg\n");
			store_reg(f, p->q1.reg, &p->z, regtype[p->q1.reg]->flags);
			continue;
		}
		// Reject types we can't handle - anything beyond a pointer and chars with more than 1 byte.
//		if ((c == PUSH)
//		    && ((t & NQ) > POINTER || ((t & NQ) == CHAR && zm2l(p->q2.val.vmax) != 1))) {
//			printf("Pushing a type we don't yet handle: 0x%x\n", t);
//			ierror(0);
//		}

		if ((c == ASSIGN) && ((t & NQ) > UNION)) {
			printf("Assignment of a type we don't yet handle: 0x%x\n", t);
			ierror(0);
		}

		// Avoid stack top slot trickery if the operation involves pushing operands to the stack
	    if(c==DIV || c==MOD ||
			((c==ASSIGN || c==PUSH) && ((t & NQ) > POINTER || ((t & NQ) == CHAR && zm2l(p->q2.val.vmax) != 1))))
			p = preload(f, p, 0);	// Setup zreg, etc.
		else
			p = preload(f, p, 1);	// Setup zreg, etc.

		c = p->code;

		if (c == SUBPFP)
			c = SUB;
		if (c == ADDI2P)
			c = ADD;
		if (c == SUBIFP)
			c = SUB;

//		emit(f, "// code 0x%x, q1->v: %x\n", c,&p->q1.v);
//		if(p->prev && matchobj(f,&p->q1,&p->prev->q1))
//			emit(f, "// Matching objs found\n", p->prev->code,&p->prev->q1.v);

		if (c == CONVERT) {
			if(DBGMSG)
				emit(f, "\t\t\t\t\t\t//FIXME convert\n");
			if (ISFLOAT(q1typ(p)) || ISFLOAT(ztyp(p))) {
				printf("Float not yet supported\n");
				ierror(0);
			}
			if (sizetab[q1typ(p) & NQ] < sizetab[ztyp(p) & NQ]) {
				int shamt = 0;
				if(DBGMSG)
					emit(f,"\t\t\t\t\t\t//Converting to wider type...\n");
				switch (q1typ(p) & NU) {
					case CHAR | UNSIGNED:
					case SHORT | UNSIGNED:
						if(DBGMSG)
							emit(f,"\t\t\t\t\t\t//But unsigned, so no need to extend\n");

						if(involvesreg(z)) {
							emit_prepobj(f, &p->z, ztyp(p), zreg, 0);
							emit_objtoreg(f, &p->q1, q1typ(p), tmp);
							save_temp(f, p, zreg);
							// WARNING - might need to invalidate temp objects here...
						} else {
							emit_objtoreg(f, &p->q1, q1typ(p), zreg);
							save_result(f, p);
							// WARNING - might need to invalidate temp objects here...
						}
						break;
					case CHAR:
						emit_objtoreg(f, &p->q1, q1typ(p), zreg);
						emit_constanttotemp(f,0xffffff80);
						emit(f,"\tadd\t%s\n",regnames[zreg]);
						emit(f,"\txor\t%s\n",regnames[zreg]);
						cleartempobj(f,zreg);
						save_result(f, p);
						break;
					case SHORT:
						emit_objtoreg(f, &p->q1, q1typ(p), zreg);
						emit_constanttotemp(f,0xffff8000);
						emit(f,"\tadd\t%s\n",regnames[zreg]);
						emit(f,"\txor\t%s\n",regnames[zreg]);
						cleartempobj(f,zreg);
						save_result(f, p);
						break;
				}
//				settempobj(f,zreg,&p->z,0,0);
			} else if(sizetab[q1typ(p) & NQ] >= sizetab[ztyp(p) & NQ]) {	// Reducing the size, must mask off excess bits...
				if(DBGMSG)
					emit(f,"\t\t\t\t\t\t// (convert - reducing type %x to %x\n",q1typ(p),ztyp(p));

				// If Z is not a register then we're storing a halfword or byte, and thus don't need to mask...

				if(((p->q1.flags&(REG|DREFOBJ))==REG) && (p->z.flags&(REG|DREFOBJ))!=REG) {
					if(p->z.flags&DREFOBJ) {	// Can't use stmpdec for dereferenced objects
						emit_prepobj(f, &p->z, t, zreg, 0); // Need an offset
						emit_objtoreg(f, &p->q1, q1typ(p), tmp);
						save_temp(f,p,zreg);
#if 0
						emit_prepobj(f, &p->z, t, tmp, 0); // Need an offset
						emit(f, "\texg\t%s\n", regnames[q1reg]);
//						if(!isstackparam(&p->z) || (p->z.flags&DREFOBJ))
							emit_sizemod(f,ztyp(p));
						emit(f, "\tst\t%s\n", regnames[q1reg]);
						if(p->z.am && p->z.am->disposable && p->q1.am && p->q1.am->disposable)
							emit(f, "\t\t\t\t\t\t// Both q1 and z are disposable, not bothering to undo exg\n");
						else
							emit(f, "\texg\t%s\n", regnames[q1reg]);
#endif
					}
					else {
						// Use stmpdec if q1 is already in a register...
						emit_prepobj(f, &p->z, ztyp(p), tmp, 4); // Need an offset
						if(!isstackparam(&p->z))
							emit_sizemod(f,ztyp(p));
						emit(f,"\tstmpdec\t%s\n",regnames[q1reg]);
					}
				}
				else { // Destination is a register - we must mask...
					// Potential optimisation here - track which ops could have caused a value to require truncation.
					// Also figure out what's happening next to the value.  If it's only being added, anded, ored, xored
					// and then truncated by a write to memory we don't need to worry.
					if(!isreg(q1) || !isreg(z) || q1reg!=zreg) // Do we just need to mask in place, or move the value first?
					{
						if(!isreg(z))
							zreg=t1;
						emit_prepobj(f, &p->z, ztyp(p), t1, 0);

						emit_objtoreg(f, &p->q1, t,tmp);
						emit(f,"\t\t\t\t\t\t//Saving to reg %s\n",regnames[zreg]);
						save_temp(f, p, zreg);
					}
//					else
					if(zreg!=sp && (p->z.flags&(DREFOBJ|REG))==REG)
					{
						switch(ztyp(p)&NQ) {
							case SHORT:
								emit_constanttotemp(f, 0xffff);
								emit(f, "\tand\t%s\n", regnames[zreg]);
								break;
							case CHAR:
								emit_constanttotemp(f, 0xff);
								emit(f, "\tand\t%s\n", regnames[zreg]);
								break;
							default:
								emit(f,"\t\t\t\t\t\t//No need to mask - same size\n");
								break;
						}
					}
				}
			}
			continue;
		}

		if (c == KOMPLEMENT) {
			if(DBGMSG)
				emit(f, "\t\t\t\t\t\t//comp\n");
			emit_objtoreg(f, &p->q1, q1typ(p), zreg);
			emit_constanttotemp(f,-1);
			emit(f, "\txor\t%s\n", regnames[zreg]);
//			cleartempobj(f,zreg);
			save_result(f, p);
			continue;
		}
		// May not need to actually load the register here - certainly check before emitting code.
		if (c == SETRETURN) {
			if(DBGMSG)
				emit(f, "\t\t\t\t\t\t//setreturn\n");
			emit_objtoreg(f, &p->q1, q1typ(p), zreg);
			BSET(regs_modified, p->z.reg);
			continue;
		}
		// Investigate - May not be needed for register mode?
		if (c == GETRETURN) {
			if(DBGMSG)
				emit(f, "\t\t\t\t\t\t// (getreturn)");
			if (p->q1.reg) {
				zreg = p->q1.reg;
				save_result(f, p);
			} else {
				if(DBGMSG)
					emit(f, " not reg\n");
				p->z.flags = 0;
			}
			continue;
		}
		// OK - figure out what the bvunite stuff is all about.
		if (c == CALL) {
			int reg;
			if(DBGMSG)
				emit(f, "\t\t\t\t\t\t//call\n");
			if ((p->q1.flags & (VAR | DREFOBJ)) == VAR && p->q1.v->fi && p->q1.v->fi->inline_asm) {
				emit_inline_asm(f, p->q1.v->fi->inline_asm);
				cleartempobj(f,t1);
				cleartempobj(f,tmp);
				/* FIXME - restore stack from pushed arguments? */
			} else {
				/* FIXME - deal with different object types here */
				if (p->q1.v->storage_class == STATIC) {
					// FIXME - double-check that we shouldn't include an offset here.
					emit_pcreltotemp2(f, &p->q1);
					if (p->q1.flags & DREFOBJ) {
						emit(f, "\taddt\t%s\t//Deref function pointer\n", regnames[pc]);
						emit(f, "\tldt\n\texg\t%s\n", regnames[pc]);
					} else
						emit(f, "\tadd\t%s\n", regnames[pc]);
				} else if (p->q1.v->storage_class == EXTERN) {
					if (p->q1.flags & DREFOBJ) {	// Is this a function pointer?
						emit_externtotemp(f, p->q1.v->identifier, p->q1.val.vmax);
						emit(f, "\tldt\t// deref function ptr\n");
						emit(f, "\texg\t%s\n", regnames[pc]);
					}
					else {
						emit_pcreltotemp2(f, &p->q1);
						emit(f, "\tadd\t%s\n", regnames[pc]);
					}
				} else {
					emit_objtoreg(f, &p->q1, t, tmp);
					emit(f, "\texg\t%s\n", regnames[pc]);
				}

				cleartempobj(f,tmp);

				/* If we have an addressingmode, see if we're able to defer stack popping. */
				if(p->z.am)
				{
					switch(p->z.am->deferredpop)
					{
						/* If we couldn't defer popping due to flow control changes, we need to pop any previously
						   deferred stack entries at this point.*/
						case DEFERREDPOP_FLOWCONTROL:
							emit(f,"\t\t\t\t\t\t// Flow control - popping %d + %d bytes\n",pushedargsize(p),notyetpopped);
							if(pushedargsize(p)+notyetpopped)
							{
								emit_constanttotemp(f, pushedargsize(p)+notyetpopped);				
								emit(f, "\tadd\t%s\n", regnames[sp]);
							}
							pushed -= pushedargsize(p);
							notyetpopped=0;
							break;

						/* If we couldn't defer popping due to nested calls then we only pop this function's stack entries. */
						case DEFERREDPOP_NESTEDCALLS:
							emit(f,"\t\t\t\t\t\t// Nested call - popping %d bytes\n",pushedargsize(p));
							if(pushedargsize(p))
							{
								emit_constanttotemp(f, pushedargsize(p));
								emit(f, "\tadd\t%s\n", regnames[sp]);
							}
							pushed -= pushedargsize(p);
							break;

						/*	Otherwise, we're OK to defer popping until later. */
						case DEFERREDPOP_OK:
							notyetpopped+=pushedargsize(p);
							pushed -= pushedargsize(p);
							emit(f,"\t\t\t\t\t\t// Deferred popping of %d bytes (%d in total)\n",pushedargsize(p),notyetpopped);
							break;
					}
				}
				else if(pushedargsize(p))
				{
					emit_constanttotemp(f, pushedargsize(p));
					pushed -= pushedargsize(p);
					emit(f, "\tadd\t%s\n", regnames[sp]);
				}
//				cleartempobj(f,tmp);
				cleartempobj(f,t1);
			}
			 /*FIXME*/
			if ((p->q1.flags & (VAR | DREFOBJ)) == VAR && p->q1.v->fi
				       && (p->q1.v->fi->flags & ALL_REGS)) {
				bvunite(regs_modified, p->q1.v->fi->regs_modified, RSIZE);
			} else {
				int i;
				for (i = 1; i <= MAXR; i++) {
					if (regscratch[i])
						BSET(regs_modified, i);
				}
			}
			continue;
		}

		if ((c == ASSIGN || c == PUSH) && t == 0) {
			printf("Bad type for assign / push\n");
			ierror(0);
		}
		// Basically OK.
		if (c == PUSH) {
			int matchreg;
			if(DBGMSG)
				emit(f, "\t\t\t\t\t\t// (a/p push)\n");

			/* Handle composite types */
		    if((t & NQ) > POINTER || ((t & NQ) == CHAR && zm2l(p->q2.val.vmax) != 1)) {
//				if(DBGMSG)
					emit(f,"\t\t\t\t\t\t// Pushing composite type - size %d, pushed size %d\n",opsize(p),pushsize(p));
				emit_inlinepush(f,p,t);
				pushed += pushsize(p);
			}
			else
			{
				/* need to take dt into account */
				if(DBGMSG)
					emit(f, "\t\t\t\t\t\t// a: pushed %ld, regnames[sp] %s\n", pushed, regnames[sp]);
				switch(t&NQ)
				{
					case INT:
					case LONG:
					case POINTER:
						emit_objtoreg(f, &p->q1, t, tmp);
						emit(f, "\tstdec\t%s\n", regnames[sp]);
						break;
					default:
						printf("Pushing unhandled type 0x%x to the stack\n",t);
						ierror(0);
						break;
				}
				pushed += pushsize(p);
			}
			continue;
		}

		if (c == ASSIGN) {
			if(DBGMSG)
				emit(f, "\t\t\t\t\t\t// (a/p assign)\n");
			if (((t & NQ) == STRUCT) || ((t & NQ) == UNION) || ((t & NQ) == ARRAY)
			    || ((t & NQ) == CHAR && opsize(p) != 1)) {
				emit_inlinememcpy(f,p,t);
			} else {
				// Is the small speedup here worth the complexity?  (Yes, because it improves code density)
				// Use stmpdec if q1 is already in a register and we're not using addressing modes...
				if(!check_am(p) && ((p->q1.flags&(REG|DREFOBJ))==REG) && !(p->z.flags&REG))
				{
					if(p->z.flags&DREFOBJ)	// Can't use stmpdec for dereferenced objects
					{
						emit_prepobj(f, &p->z, t, tmp, 0);
						emit(f, "\texg\t%s\n", regnames[q1reg]);
						emit_sizemod(f,t);
						emit(f, "\tst\t%s\n", regnames[q1reg]);
						if(p->z.am && p->z.am->disposable)
						{
							cleartempobj(f,tmp);
							emit(f, "\t\t\t\t\t\t// Object is disposable, not bothering to undo exg\n");
						}
						else
							emit(f, "\texg\t%s\n", regnames[q1reg]);
					}
					else
					{
						emit_prepobj(f, &p->z, t, tmp, 4); // Need an offset
						if(!isstackparam(&p->z))
							emit_sizemod(f,t);
						emit(f,"\tstmpdec\t%s\n",regnames[q1reg]);
						cleartempobj(f,tmp);
					}
				}
				else
				{
					emit_prepobj(f, &p->z, t, t1, 0);
					emit_objtoreg(f, &p->q1, t, tmp);
					save_temp(f, p, t1);
				}
			}
			continue;
		}
		// Seems to work.
		if (c == ADDRESS) {
			if(DBGMSG)
				emit(f, "\t\t\t\t\t\t// (address)\n");
			if(involvesreg(z))
			{
				emit_prepobj(f, &p->q1, POINTER, tmp, 0);
				save_temp(f,p,zreg);
			}
			else
			{
				emit_prepobj(f, &p->q1, POINTER, zreg, 0);
				save_result(f, p);
			}
			continue;
		}
		// OK
		if (c == MINUS) {
			if(DBGMSG)
				emit(f, "\t\t\t\t\t\t// (minus)\n");
			emit_objtoreg(f, &p->q1, q1typ(p), zreg);
			emit_constanttotemp(f,0);
			emit(f, "\texg %s\n\tsub %s\n", regnames[zreg], regnames[zreg]);
			settempobj(f,tmp,&p->q1,0,0); // Temp contains un-negated value
			// cleartempobj(f,tmp);
			save_result(f, p);
			continue;
		}
		// Compare - #
		// Revisit
		if (c == TEST) {
			if(DBGMSG)
				emit(f, "\t\t\t\t\t\t// (test)\n");
			if(!emit_objtoreg(f, &p->q1, t, tmp)) /* emit_objtoreg might already have set the Z flag */
			{
				emit(f,"\t\t\t\t// flags %x\n",p->q1.flags);
				if ((p->q1.flags & (REG|DREFOBJ)) == REG)	// Can avoid mr if the value came from a register
						emit(f, "\tand\t%s\n", regnames[p->q1.reg]);
				else
				{
						emit(f, "\tmr\t%s\n\tand\t%s\n", regnames[t1], regnames[t1]);
						settempobj(f,t1,&p->q1,0,0);
				}
//				cleartempobj(f,tmp);
//				cleartempobj(f,t1);
			}
			continue;
		}
		// Compare
		// Revisit
		if (c == COMPARE) {
			if(DBGMSG)
			{
					emit(f, "\t\t\t\t\t\t// (compare)");
				if (q1typ(p) & UNSIGNED)
					emit(f, " (q1 unsigned)");
				else
					emit(f, " (q1 signed)");
				if (q2typ(p) & UNSIGNED)
					emit(f, " (q2 unsigned)");
				else
					emit(f, " (q2 signed)");
				emit(f,"\n");
			}

			// If q2 is a register but q1 isn't we could reverse the comparison, but would then have to reverse
			// the subsequent conditional branch.
			// FIXME - can also reverse if one value is cached

			if (!isreg(q1)) {
				if(isreg(q2)) {	// Reverse the test.
					emit_objtoreg(f, &p->q1, t,tmp);
					q1reg=q2reg;
					reversecmp=1;
				} else { // Neither object is in a register, so load q1 into t1 and q2 into tmp.
					emit_objtoreg(f, &p->q1, t,t1);
					cleartempobj(f,t1);
					q1reg = t1;
					emit_objtoreg(f, &p->q2, t,tmp);
				}
			}
			else
				emit_objtoreg(f, &p->q2, t,tmp);
			if ((!(q1typ(p) & UNSIGNED)) && (!(q2typ(p) & UNSIGNED))) {	// If we have a mismatch of signedness we treat as unsigned.
				int nextop=p->next->code;	// Does the sign matter for the branch being done?
				if(nextop==FREEREG)
					nextop=p->next->next->code;
				if((nextop!=BEQ) && (nextop!=BNE))
					emit(f, "\tsgn\n");	// Signed comparison
			}
			emit(f, "\tcmp\t%s\n", regnames[q1reg]);
			continue;
		}

		// Division and modulo
		if ((c == MOD) || (c == DIV)) {
			int targetreg=zreg;
			int doneq2=0;
			// FIXME - do we need to use switch_IC here?
			if(DBGMSG)
				emit(f, "\t\t\t\t\t\t//Call division routine\n");

			// determine here whether R1 and R2 really need saving - may not be in use, or may be the target register.
			if(regs[t2] && zreg!=t2)
			{
				emit(f, "\tmt\t%s\n\tstdec\t%s\n", regnames[t2], regnames[sp]);
				cleartempobj(f,tmp);
				pushed+=4;
			}
			if(regs[t2+1] && zreg!=(t2+1))
			{
				emit(f, "\tmt\t%s\n\tstdec\t%s\n", regnames[t2 + 1], regnames[sp]);
				cleartempobj(f,tmp);
				pushed += 4;
			}
			// q1 must be written to t2, q2 must be written to t2+2
			// if q2 starts in t2 we have to avoid overwriting it.

			// If q1 is already in t2, q2 can't be, so we don't need to worry about swapping
			if(!isreg(q1) || q1reg!=t2)
			{
				emit_objtoreg(f, &p->q1, t,tmp);

				// Need to make sure we're not about to overwrite the other operand!
				if(isreg(q2) && q2reg==t2)
				{
					emit(f,"\texg\t%s\n",regnames[t2]);
					emit(f,"\tmr\t%s\n",regnames[t2+1]);
					doneq2=1;
				}
				else
					emit(f, "\tmr\t%s\n", regnames[t2]);
			}
			if(!doneq2 && (!isreg(q2) || q2reg!=t2+1))
			{
				emit_objtoreg(f, &p->q2, t,tmp);
				emit(f, "\tmr\t%s\n", regnames[t2 + 1]);
			}
			cleartempobj(f,t1);
			cleartempobj(f,t2);

			if ((!(q1typ(p) & UNSIGNED)) && (!(q2typ(p) & UNSIGNED)))	// If we have a mismatch of signedness we treat as unsigned.
				emit(f, "\t.lipcrel\t_div_s32bys32\n");
			else
				emit(f, "\t.lipcrel\t_div_u32byu32\n");
			emit(f, "\tadd\t%s\n", regnames[pc]);

			// If the next IC is SetReturn from the same register we can skip saving the result.
			if(next_setreturn(p,zreg))
			{
				emit(f,"\t\t\t\t\t\t// Skipping save_result...\n");
				targetreg=t1;
			}

			if (c == MOD)
			{
				if(targetreg!=t2)
					emit(f, "\tmt\t%s\n\tmr\t%s\n", regnames[t2], regnames[zreg]);
			}
			else
			{
				if(targetreg!=t1)
					emit(f, "\tmt\t%s\n\tmr\t%s\n", regnames[t1], regnames[zreg]);
			}

			if(regs[t2+1] && zreg!=(t2+1))
			{
				emit(f, "\tldinc\t%s\n\tmr\t%s\n", regnames[sp], regnames[t2+1]);
				pushed -= 4;
			}
			if(regs[t2] && zreg!=t2)
			{
				emit(f, "\tldinc\t%s\n\tmr\t%s\n", regnames[sp], regnames[t2]);
				pushed -= 4;
			}

			cleartempobj(f,tmp);
			cleartempobj(f,t1);

			// Target not guaranteed to be a register.
			save_result(f, p);

			continue;
		}

		// Remaining arithmetic and bitwise operations

		if ((c >= OR && c <= AND) || (c >= LSHIFT && c <= MULT)) {
			if(DBGMSG)
				emit(f, "\t\t\t\t\t\t// (bitwise/arithmetic) ");
			if(DBGMSG)
				emit(f, "\t//ops: %d, %d, %d\n", q1reg, q2reg, zreg);
			if(p->q1.am && p->q1.am->type==AM_ADDT)
			{
				if(DBGMSG)
					emit(f,"\t\t\t\t\t\t//Special case - addt\n");
				// FIXME - if q2 is already in tmp could reverse this
				if(p->q2.flags&KONST)
				{
					zreg=t1;
					emit_prepobj(f, &p->z, t, t1, 0);

					emit_objtoreg(f, &p->q2, t,tmp);
					emit(f,"\taddt\t%s\n",regnames[p->q1.reg]);
					settempobj(f,tmp,&p->z,0,0);
					save_temp(f, p, zreg);
					obsoletetempobj(f,t1,&p->z,0);
//					emit(f,"\tmr\t%s\n",regnames[p->z.reg]);
				}
				else
				{
					zreg=t1;
					emit_prepobj(f, &p->z, t, t1, 0);

					emit_objtoreg(f, &p->q1, t,tmp);
					emit(f,"\taddt\t%s\n",regnames[p->q2.reg]);
					settempobj(f,tmp,&p->z,0,0);
					save_temp(f, p, zreg);
					obsoletetempobj(f,t1,&p->z,0);
//					emit(f,"\tmr\t%s\n",regnames[p->z.reg]);
				}
				continue;
			}

			if (involvesreg(q2) && q2reg == zreg) {
//                      printf("Target register and q2 are the same!  Attempting a switch...\n");
				if (switch_IC(p)) {
					preload(f,p,1);	// refresh q1reg, etc after switching the IC
				} else {
					emit(f,
					     "\t\t\t\t\t\t// WARNING - evading q2 and target collision - check code for correctness.\n");
					zreg = t1;
				}
			}
			if (involvesreg(q1) && q1reg == zreg)
				emit(f,"\t\t\t\t\t\t// WARNING - q1 and target collision - check code for correctness.\n");

			if (!isreg(q1) || q1reg != zreg) {
				emit_objtoreg(f, &p->q1, t,zreg);
//				emit(f, "\tmr\t%s\n", regnames[zreg]);	// FIXME - what happens if zreg and q1/2 are the same?
			}
			emit_objtoreg(f, &p->q2, t,tmp);
			if (c >= OR && c <= AND)
				emit(f, "\t%s\t%s\n", logicals[c - OR], regnames[zreg]);
			else {
				if (c == RSHIFT || c==MULT)	// Modify right shift operations with appropriate signedness...
				{
//					printf("q1typ: %x, q2typ: %x, ztyp: %x\n",q1typ(p),q2typ(p),ztyp(p));
					if (!(t & UNSIGNED))
					{
						// Evaluate q1 - if we're dealing with a constant that doesn't have bit 31 set we don't need sgn...
						if((!(p->typf2 & UNSIGNED) && c==RSHIFT) 
									|| (p->q1.flags&(KONST|DREFOBJ)!=KONST)
											|| (val2zmax(&p->q1,p->typf)&0x80000000))
							emit(f, "\tsgn\n");
					}
				}
				emit(f, "\t%s\t%s\n", arithmetics[c - LSHIFT], regnames[zreg]);
				if(c==MULT)
					cleartempobj(f,tmp);
			}
			settempobj(f,zreg,&p->z,0,0);
			cleartempobj(f,zreg);
			save_result(f, p);
			continue;
		}
		printf("Unhandled IC\n");
		pric2(stdout, p);
		ierror(0);
	}
	if(function_bottom(f, v, localsize+notyetpopped,firsttail))
		firsttail=0;
}

int shortcut(int code, int typ)
{
	// Only RSHIFT and AND are safe on 832.
	// So far have seen shortcut requests for
	// DIV
	// ADD
	// RSHIFT
	// COMPARE
	// SUB
	// LSHIFT
	// AND
	// MULT
	// OR

//	printf("Evaluating shortcut for code %d, type %x\n",code,typ);
	if(code==RSHIFT)
		return(1);
	if(code==AND)
		return(1);

	return 0;
}

int reg_parm(struct reg_handle *m, struct Typ *t, int vararg, struct Typ *d)
{
	int f;
	f = t->flags & NQ;
	if(is_varargs(d))	/* Disallow register parameters for varargs functions */
		return(0);

	if (f <= LONG || f == POINTER) {
		if (m->gregs >= REGPARM_COUNT)
			return 0;
		else
			return FIRST_GPR + 1 + m->gregs++;
	}
	if (ISFLOAT(f)) {
		return(0);
/*		if (m->fregs >= 0)
			return 0;
		else
			return FIRST_FPR + 2 + m->fregs++;
*/
	}
	return 0;
}

int iscomment(char *str)
{
	char c;
	while(c=*str++)
	{
		if(!c || c=='\n' || c=='/')
			return(1);
		if(c!=' '&&c!='\t')
			return(0);
	}
	return(1);
}


int emit_peephole(void)
{
	int i;
	int havemr=0;
	int havemt=0;
	int havestore=0;
	int haveload=0;
	int loadidx=0;
	i=emit_f;

	while(i!=emit_l)
	{
		int reg,reg2;
	    if(sscanf(emit_buffer[i],"\tmr\tr%d",&reg)==1)
		{
			if(havemt && reg==reg2)
			{
				strcpy(emit_buffer[i],"\t//mr\n");
				return(1);
			}
			reg2=reg;
			havemr=1;
			havemt=0;
		}
	    else if(sscanf(emit_buffer[i],"\tmt\tr%d",&reg)==1)
		{
			if(havemr && reg==reg2)
			{
				strcpy(emit_buffer[i],"\t//mt\n");
				return(1);
			}
			reg2=reg;
			havemr=0;
			havemt=1;
		}
		else if(sscanf(emit_buffer[i],"\tst\tr%d",&reg)==1)
		{
			havemr=havemt=0;
			havestore=1;
		}
		else if(sscanf(emit_buffer[i],"\tld\tr%d",&reg2)==1 && havestore)
		{
			havemr=havemt=0;
			if(reg==reg2 && reg==6) /* Only stack ops - others would be risky due to potential hardware registers. */
			{
				loadidx=i;
				haveload=1;
//				printf("Found matching load directive, r%d\n",reg);
//				strcpy(emit_buffer[i],"\t//nop\n");
//				return(1);
			}
		}
		else if(!iscomment(emit_buffer[i])) /* Check that the next instruction isn't "cond" */
		{
			havemr=havemt=0;
			if(haveload && strncmp(emit_buffer[i],"\tcond",5)) /* If not, we're OK to zero out the load */
			{
				strcpy(emit_buffer[loadidx],"\t//nop\n");
				return(1);
			}
			else
			{
				havestore=haveload=0;
			}
		}
		i=(i+1)%EMIT_BUF_DEPTH;
	}
	return 0;                                                                    
}


int handle_pragma(const char *s)
{
	return(0);
}

void cleanup_cg(FILE * f)
{
}

void cleanup_db(FILE * f)
{
	if (f)
		section = -1;
}
