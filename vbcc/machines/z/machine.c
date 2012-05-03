/* z/machine.c
 * Code generator for the Z-machine.
 * (C) David Given 2001
 * conversion to vbcc 0.8 by Volker Barthelmann
 */

/* This code is licensed under the MIT open source license.
 *
 * Copyright (c) 2001, David Given
 * All rights reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

/* This code generator produces code for the Z-machine. The Z-machine is the
 * highly peculiar virtual machine used for the old Infocom text adventures;
 * these days, an extended form is still used in the interactive fiction genre
 * of games. Usually, a dedicated compiler called Inform is used to generate
 * code, but it would be nice to be able to use real C, so here we are.
 *
 * The Z-machine is (mostly) a semi-stack-based Harvard architecture machine.
 * (Split code and data space, although they can share if you're clever. And
 * mad.) It has no registers, but it does have procedure local variables which
 * will do instead. It has a dedicated stack but as it's not accessible by
 * ordinary memory it's not useful for C. It uses 8 and 16 bit words, so we'll
 * have to emulate 32-bit arithmetic.
 *
 * For more information, including code for Inform, various interpreters, more
 * documentation than you can shake a stick at, and the full technical reference
 * for the Z-machine, check out the Interactive Fiction archive, at
 * http://www.ifarchive.org.
 *
 * Things to note: there is no Z-machine assembler. (Well, there's zasm, but it's
 * really just a rumour.) Luckily, Inform has an assembler mode, where it'll
 * generate raw Z-machine opcodes. Unluckily, it's horribly buggy... So we're
 * going to have to generate Inform source, which seems at first to be rather
 * silly, but as Inform is quite a simple compiler we can make sure that it's
 * only going to generate the instructions we want it to generate.
 */

/* vbcc-mandated header. */

#include "supp.h"
static char FILE_[]=__FILE__;
char cg_copyright[]="vbcc code-generator for Z-machine V0.0a (c) in 2001 by David Given";

/* Command-line flags. */

int g_flags[MAXGF] = {
	STRINGFLAG,
	0,
	0,
	0,
	0,
	0
};
char *g_flags_name[MAXGF] = {
	"module-name",
	"trace-calls",
	"trace-all",
	"safe-branches",
	"comment-ic",
	"comment-misc"
};
union ppi g_flags_val[MAXGF];

/* Type alignment. Much better code is generated if we can use even alignment.
 */

zmax align[MAX_TYPE+1] = {
	0,	/* 0: unused */
	1,	/* 1: CHAR */
	2,	/* 2: SHORT */
	2, 	/* 3: INT */
	2,	/* 4: LONG */
	2,      /* 5: LLONG */
	2,	/* 6: FLOAT */
	2,	/* 7: DOUBLE */
	2,      /* 8: LDOUBLE */
	2,	/* 9: VOID */
	2,	/* 10: POINTER */
	1,	/* 11: ARRAY */
	1,	/* 12: STRUCT */
	1,	/* 13: UNION */
	1,	/* 14: ENUM */
	1,	/* 15: FUNKT */
};

/* Alignment that is valid for all types. */

zmax maxalign = 2;

/* Number of bits in a char. */

zmax char_bit = 8;

/* Sizes of all elementary types, in bytes. */

zmax sizetab[MAX_TYPE+1] = {
	0,	/* 0: unused */
	1,	/* 1: CHAR */
	2,	/* 2: SHORT */
	2, 	/* 3: INT */
	4,	/* 4: LONG */
        8,      /* 5: LLONG */
	4,	/* 6: FLOAT */
	8,	/* 7: DOUBLE */
	8,      /* 8: LDOUBLE */
	0,	/* 9: VOID */
	2,	/* 10: POINTER */
	0,	/* 11: ARRAY */
	0,	/* 12: STRUCT */
	0,	/* 13: UNION */
	2,	/* 14: ENUM */
	0,	/* 15: FUNKT */
};

/*  Minimum and Maximum values each type can have.              */
/*  Must be initialized in init_cg().                           */
zmax t_min[MAX_TYPE+1];
zumax t_max[MAX_TYPE+1];
zumax tu_max[MAX_TYPE+1];

/* Names of all the registers.
 * We can have 16 local variables per routine. Var 0 is always the C stack
 * pointer, xp. All the others can be used by the compiler. xp doesn't actually
 * appear in the register map, so we get 15 main registers. 
 */

char* regnames[] = {
	"sp", /* vbcc doesn't use this, but we do */
	"xp", 	"r0", 	"r1",  	"r2", 	"r3", 	"r4", 	"r5", 	"r6",
	"r7", 	"r8", 	"r9",	"r10",	"r11",	"r12"};
#define XP 1
#define USERREG 2

/* The size of each register, in byes. */

zmax regsize[] = {
	0,
	2,	2,	2,	2,	2,	2,	2,	2,
	2,	2,	2,	2,	2,	2};

/* Type needed to store each register. */

struct Typ ityp = {INT};
struct Typ* regtype[] = {
	NULL,
	&ityp,	&ityp,	&ityp,	&ityp,	&ityp,	&ityp,	&ityp,	&ityp,
	&ityp,	&ityp,	&ityp,	&ityp,	&ityp,	&ityp};
	
/* These registers are those dedicated for use by the backend. These ones will
 * not be used by the code generator. */

int regsa[] = {
	0,
	1,	0,	0,	0,	0,	0,	0,	0,
	0,	0,	0,	0,	0,	0};

/* Specifies which registers may be destroyed by function calls. As we're
 * storing our registers in local variables so they're being automatically
 * saved for us, none of them.
 */

int regscratch[] = {
	0,
	0,	0,	0,	0,	0,	0,	0,	0,
	0,	0,	0,	0,	0,	0};

/* Default state for register parameter passing. */

struct reg_handle empty_reg_handle =
	{USERREG};

/* Prefix for labels. */

static char* labelprefix = "L";

/* Name of the current module; used for generating unique names for statics and
 * the constant pool. */

static char* modulename;

/* Stack frame layout:
 *
 *  --------------
 *      Arg 4        (Arguments being passed to this function)
 *      Arg 3
 *      Arg 2
 *      Arg 1
 *  -------------- xp + stackparamadjust + stackoffset
 *     Local 4       (This function's temp space)
 *     Local 3
 *     Local 2
 *     Local 1
 *  -------------- xp + stackparamadjust
 *      Arg 2        (Arguments this function has pushed to pass
 *      Arg 1         to a called function)
 *  -------------- xp
 *
 * Any area may be zero in size. (Although stackoffset is always at least 2 for
 * some inadequately explained reason.)
 */

static int stackoffset;
static int stackparamadjust;

/* Represents something the Z-machine can use as an instruction operand. */

struct zop {
	int type;
	union {
		int reg;
		zmax constant;
		char* identifier;
	} val;
};

enum {
	ZOP_STACK,
	ZOP_REG,
	ZOP_CONSTANT,
	ZOP_EXTERN,
	ZOP_STATIC,
	ZOP_CONSTANTADDR
};

/* Some useful zops. */

struct zop zop_zero = {ZOP_CONSTANT, {constant: 0}};
struct zop zop_xp = {ZOP_REG, {reg: XP}};
struct zop zop_stack = {ZOP_STACK, 0};

/* Temporaries used to store comparison register numbers. */

static struct zop compare1;
static struct zop compare2;

/* Keeps track of whether we've emitted anything or not. Used to determine
 * whether to emit the seperating ; or not. If it's 1, we haven't emitted
 * anything. If it's -1, we're doing an array, so we need to emit a final (0)
 * to finish it off if it's only one byte long. 0 for anything else. */

static int virgin = 1;

/* The current variable we're emitting data for. */

struct variable {
	int type;
	union {
		char* identifier;
		int number;
	} val;
	zint offset;
};

struct variable currentvar;

/* Inform can't emit variable references inside arrays. So when vbcc wants to
 * put, say, the address of something in a global variable, we have to write it
 * in later. A linked list of these structures keeps track of the items that
 * need fixing up. */

struct fixup {
	struct fixup* next;
	struct variable identifier;
	struct variable value;
	zmax offset;
};

static struct fixup* fixuplist = NULL;
	
/* 32-bit values are stored in a constant pool, for simplicity. It's kept track
 * of in this linked list. */

struct constant {
	struct constant* next;
	int id;
	zmax value;
};

static struct constant* constantlist = NULL;
static int constantnum = 0;

/* The function we're currently compiling. */

static struct Var* function;

/* Function prototypes. */

static void emit_add(FILE* fp, struct zop* q1, struct zop* q2, struct zop* z);
static void read_reg(FILE* fp, struct obj* obj, int typf, int reg);
static int addconstant(zmax value);

/* Emit debugging info. */

static void debugemit(FILE* fp, char* fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	if (g_flags[5] & USEDFLAG)
		vfprintf(fp, fmt, ap);
	va_end(ap);
}

/* Do we need to emit a ; before the next thing? */

static void reflower(FILE* fp)
{
	if (!virgin)
		fprintf(fp, ";\n");
	if (virgin == -1)
	{
		if (currentvar.offset == 1)
			fprintf(fp, "(0)");
		fprintf(fp, ";");
	}
	virgin = 0;
}

/* Extract the sign extended byte n of a value. */

static char xbyte(zmax val, int byte)
{
	val <<= (sizeof(val)*8) - (byte*8) - 8;
	val >>= (sizeof(val)*8) - 8;
	return (unsigned char) val;
}

/* Extract the sign extended word n of a value. */

static zshort xword(zmax val, int word)
{
	val <<= (sizeof(val)*8) - (word*16) - 16;
	val >>= (sizeof(val)*8) - 16;
	return (zshort) val;
}

/* Debug function: prints the text name of a type. */

static void dump_type(FILE* fp, int typf)
{
	switch (typf)
	{
		case VOID:	fprintf(fp, "VOID"); break;
		case CHAR:	fprintf(fp, "CHAR"); break;
		case SHORT:	fprintf(fp, "SHORT"); break;
		case INT:	fprintf(fp, "INT"); break;
		case LONG:	fprintf(fp, "LONG"); break;
		case POINTER:	fprintf(fp, "POINTER"); break;
		case STRUCT:	fprintf(fp, "STRUCT"); break;
		case ARRAY:	fprintf(fp, "ARRAY"); break;
		case UNION:	fprintf(fp, "UNION"); break;
		case FUNKT:	fprintf(fp, "FUNKT"); break;
		default:	fprintf(fp, "unknown %X", typf);
	}
}

/* Debug function: outputs the obj. */

static void dump_obj(FILE* fp, struct obj* obj, int typf)
{
	int f = obj->flags & (KONST|REG|VAR|DREFOBJ|VARADR);

	if (f == 0)
	{
		fprintf(fp, "[]");
		return;
	}

	if (f & DREFOBJ)
		fprintf(fp, "*");

	if (f & VARADR)
		fprintf(fp, "&");

	if (f == KONST)
	{
		switch (typf & NU)
		{
			case CHAR:
				fprintf(fp, "[char #%d]", obj->val.vchar);
				break;

			case UNSIGNED|CHAR:
				fprintf(fp, "[uchar #%u]", obj->val.vuchar);
				break;

			case SHORT:
				fprintf(fp, "[short #%d]", obj->val.vshort);
				break;

			case UNSIGNED|SHORT:
				fprintf(fp, "[ushort #%u]", obj->val.vushort);
				break;

			case INT:
				fprintf(fp, "[int #%d]", obj->val.vint);
				break;

			case UNSIGNED|INT:
				fprintf(fp, "[uint #%d]", obj->val.vuint);
				break;

			case LONG:
				fprintf(fp, "[long #%d]", obj->val.vlong);
				break;

			case UNSIGNED|LONG:
				fprintf(fp, "[ulong #%u]", obj->val.vulong);
				break;

			case FLOAT:
				fprintf(fp, "[float #%04X]", obj->val.vfloat);
				break;

			case DOUBLE:
				fprintf(fp, "[double #%08X]", obj->val.vdouble);
				break;
#if 0
			case POINTER:
				fprintf(fp, "[pointer #%04X]", obj->val.vpointer);
				break;
#endif
		}
	}
	else if (f == REG)
		fprintf(fp, "[reg %s]", regnames[obj->reg]);
	else if (f == (REG|DREFOBJ))
		fprintf(fp, "[deref reg %s]", regnames[obj->reg]);
	//else if (f & VAR)
	else
	{
		fprintf(fp, "[var ");
		dump_type(fp, typf);
		fprintf(fp, " %s", obj->v->identifier);

		if ((obj->v->storage_class == AUTO) ||
		    (obj->v->storage_class == REGISTER))
		{
			zmax offset = obj->v->offset;
			//if (offset < 0)
			//	offset = -(offset+maxalign);
			fprintf(fp, " at fp%+d", offset);
		}

		fprintf(fp, "+%ld", obj->val.vlong);

		if (f & REG)
			fprintf(fp, " in %s", regnames[obj->reg]);
		fprintf(fp, "]");
	}
}

/* Debug function: outputs the ic, as a comment. */

static void dump_ic(FILE* fp, struct IC* ic)
{
	char* p;

	if (!ic)
		return;

	if (!(g_flags[4] & USEDFLAG))
		return;

	if (g_flags[2] & USEDFLAG)
		fprintf(fp, "print \"");
	else
		fprintf(fp, "! ");
	
	switch (ic->code)
	{
		case ASSIGN:		p = "ASSIGN";		break;
		case OR:		p = "OR";		break;
		case XOR:		p = "XOR";		break;
		case AND:		p = "AND";		break;
		case LSHIFT:		p = "LSHIFT";		break;
		case RSHIFT:		p = "RSHIFT";		break;
		case ADD:		p = "ADD";		break;
		case SUB:		p = "SUB";		break;
		case MULT:		p = "MULT";		break;
		case DIV:		p = "DIV";		break;
		case MOD:		p = "MOD";		break;
		case KOMPLEMENT:	p = "KOMPLEMENT";	break;
		case MINUS:		p = "MINUS";		break;
		case ADDRESS:		p = "ADDRESS";		break;
		case CALL:		p = "CALL";		break;
#if 0
		case CONVCHAR:		p = "CONVCHAR";		break;
		case CONVSHORT:		p = "CONVSHORT";	break;
		case CONVINT:		p = "CONVINT";		break;
		case CONVLONG:		p = "CONVLONG";		break;
		case CONVFLOAT:		p = "CONVFLOAT";	break;
		case CONVDOUBLE:	p = "CONVDOUBLE";	break;
		case CONVPOINTER:	p = "CONVPOINTER";	break;
		case CONVUCHAR:		p = "CONVUCHAR";	break;
		case CONVUSHORT:	p = "CONVUSHORT";	break;
		case CONVUINT:		p = "CONVUINT";		break;
		case CONVULONG:		p = "CONVULONG";	break;
#endif
		case ALLOCREG:		p = "ALLOCREG";		break;
		case FREEREG:		p = "FREEREG";		break;
		case COMPARE:		p = "COMPARE";		break;
		case TEST:		p = "TEST";		break;
		case LABEL:		p = "LABEL";		break;
		case BEQ:		p = "BEQ";		break;
		case BNE:		p = "BNE";		break;
		case BLT:		p = "BLT";		break;
		case BGT:		p = "BGT";		break;
		case BLE:		p = "BLE";		break;
		case BGE:		p = "BGE";		break;
		case BRA:		p = "BRA";		break;
		case PUSH:		p = "PUSH";		break;
		case ADDI2P:		p = "ADDI2P";		break;
		case SUBIFP:		p = "SUBIFP";		break;
		case SUBPFP:		p = "SUBPFP";		break;
		case GETRETURN:		p = "GETRETURN";	break;
		case SETRETURN:		p = "SETRETURN";	break;
		case MOVEFROMREG:	p = "MOVEFROMREG";	break;
		case MOVETOREG:		p = "MOVETOREG";	break;
		case NOP:		p = "NOP";		break;
		default:		p = "???";
	}

	fprintf(fp, "%s ", p);
	dump_type(fp, ic->typf);
	fprintf(fp, " ");

	switch (ic->code)
	{
		case LABEL:
		case BEQ:
		case BNE:
		case BLT:
		case BGT:
		case BLE:
		case BGE:
		case BRA:
			fprintf(fp, "%d", ic->typf);
			goto epilogue;
	}
	
	dump_obj(fp, &ic->q1, ic->typf);
	fprintf(fp, " ");
	dump_obj(fp, &ic->q2, ic->typf);
	fprintf(fp, " -> ");
	dump_obj(fp, &ic->z, ic->typf);

epilogue:
	if (g_flags[2] & USEDFLAG)
		fprintf(fp, "^\";\n");
	else
		fprintf(fp, "\n");
}

/* Initialise the code generator. This is called once. Returns 0 if things go
 * wrong. */

int init_cg(void)
{
	modulename = g_flags_val[0].p;
	if (!modulename)
		modulename = "";

  /*  Initialize the min/max-settings. Note that the types of the     */
  /*  host system may be different from the target system and you may */
  /*  only use the smallest maximum values ANSI guarantees if you     */
  /*  want to be portable.                                            */
  /*  That's the reason for the subtraction in t_min[INT]. Long could */
  /*  be unable to represent -2147483648 on the host system.          */
  t_min[CHAR]=l2zm(-128L);
  t_min[SHORT]=l2zm(-32768L);
  t_min[INT]=t_min[SHORT];
  t_min[LONG]=zmsub(l2zm(-2147483647L),l2zm(1L));
  t_min[LLONG]=zmlshift(l2zm(1L),l2zm(63L));
  t_min[MAXINT]=t_min(LLONG);
  t_max[CHAR]=ul2zum(127L);
  t_max[SHORT]=ul2zum(32767UL);
  t_max[INT]=t_max[SHORT];
  t_max[LONG]=ul2zum(2147483647UL);
  t_max[LLONG]=zumrshift(zumkompl(ul2zum(0UL)),ul2zum(1UL));
  t_max[MAXINT]=t_max(LLONG);
  tu_max[CHAR]=ul2zum(255UL);
  tu_max[SHORT]=ul2zum(65535UL);
  tu_max[INT]=tu_max[SHORT];
  tu_max[LONG]=ul2zum(4294967295UL);
  tu_max[LLONG]=zumkompl(ul2zum(0UL));
  tu_max[MAXINT]=t_max(UNSIGNED|LLONG);

	return 1;
}

/* Returns the register in which variables of type typ are returned (or 0 if it
 * can't be done). */

int freturn(struct Typ *typ)
{
	int s = sizetab[typ->flags & NQ];
	if ((typ->flags & NQ) == VOID)
		return USERREG;
	if ((s <= sizetab[INT]) && (s > 0))
		return USERREG;
	return 0;
}

/* Returns 1 if register reg can store variables of type typ. mode is set
 * if the register is a pointer and the register is going to be dereferenced.
 */

int regok(int reg, int typf, int mode)
{
	int s = sizetab[typf & NQ];
	if ((typf & NQ) == VOID)
		return 1;
	if ((s <= sizetab[INT]) && (s > 0))
		return 1;
	return 0;
}

/* Returns zero if the IC ic can be safely executed without danger of
 * exceptions or similar things; for example, divisions or pointer dereferences
 * are dangerous. This is used by the optimiser for code reordering.
 */

int dangerous_IC(struct IC *ic)
{
	/* Check for dereferences. */

	if ((ic->q1.flags & DREFOBJ) ||
	    (ic->q2.flags & DREFOBJ) ||
	    (ic->z.flags & DREFOBJ))
		return 0;

	/* Division or modulo? */

	if ((ic->code == DIV) ||
	    (ic->code == MOD))
		return 0;

	/* Safe, as far as we can tell. */

	return 1;
}

/* Returns zero if the code for converting type p->ntyp to type typ is a noop.
 */

int must_convert(int otyp, int typ,int const_expr)
{
	int oldtype = otyp & NQ;
	int newtype = typ & NQ;

	/* ints and shorts are equivalent. */

	if (oldtype == SHORT)
		oldtype = INT;
	if (newtype == SHORT)
		newtype = INT;

	/* Both the same type? */

	if (oldtype == newtype)
		return 0;

#if 0
	/* Converting two basic integers? */

	if ((oldtype <= INT) && (newtype <= INT))
	{
		/* ... but char to short needs an AND. */

		if ((oldtype == CHAR) && (newtype != CHAR))
			return 1;
		return 0;
	}
#endif

	/* Pointer to/from int? */

	if (((oldtype == INT) || (oldtype == POINTER)) &&
	    ((newtype == INT) || (newtype == POINTER)))
		return 0;

	/* Everything else needs code. */

	return 1;
}

/* Ensure the output is aligned. A noop on the Z-machine. */

void gen_align(FILE* fp, zmax align)
{
}

/* Generate the label part of a variable definition. */

void gen_var_head(FILE* fp, struct Var* var)
{
	if (var->storage_class == EXTERN)
		debugemit(fp, "! Var %s %X\n", var->identifier, var->flags);
	if (var->storage_class == STATIC)
		debugemit(fp, "! Var static %ld %s %X\n", var->offset, var->identifier, var->flags);

	/* We only want to emit records for genuinely defined variables. For
	 * some reason, TENTATIVE is defined for some of this. */

	if ((var->storage_class == EXTERN) &&
	    !(var->flags & DEFINED) &&
	    !(var->flags & TENTATIVE))
		return;
	
	reflower(fp);
	virgin = -1;
	switch (var->storage_class)
	{
		case EXTERN:
			/* This doesn't actually mean external linkage; it
			 * means a non-static global that may be referenced
			 * externally. */
			fprintf(fp, "Array _%s ->\n",
				var->identifier);
			currentvar.type = EXTERN;
			currentvar.val.identifier = strdup(var->identifier);
			currentvar.offset = 0;
			break;
			
		case STATIC:
			fprintf(fp, "Array STATIC_%s_%ld ->\n",
				modulename, var->offset);
			currentvar.type = STATIC;
			currentvar.val.number = var->offset;
			currentvar.offset = 0;
			break;
	}
}

/* Emit a certain number of bytes of bss data. No bss on the Z-machine,
 * remember. */

void gen_ds(FILE *fp, zmax size, struct Typ *typ)
{
	fprintf(fp, " %ld\n", size);
	currentvar.offset += size;
}

/* Emit a certain number of bytes of initialised data. */

void gen_dc(FILE *fp, int typf, struct const_list *p)
{
	switch (typf & NQ)
	{
		case CHAR:
			fprintf(fp, " (%d)\n",
				p->val.vuchar);
			currentvar.offset += 1;
			break;

		case SHORT:
		case INT:
		reallyanint:
			fprintf(fp, " (%d) (%d)\n",
				xbyte(p->val.vint, 1),
				xbyte(p->val.vint, 0));
			currentvar.offset += 2;
			break;
				
		case LONG:
			fprintf(fp, " (%d) (%d) (%d) (%d)\n",
				xbyte(p->val.vlong, 3),
				xbyte(p->val.vlong, 2),
				xbyte(p->val.vlong, 1),
				xbyte(p->val.vlong, 0));
			currentvar.offset += 4;
			break;

		case POINTER:
			if (!p->tree)
				goto reallyanint;
			{
				struct fixup* fixup = malloc(sizeof(struct fixup));
				struct obj* obj = &p->tree->o;
				fixup->next = fixuplist;
				fixuplist = fixup;
				fixup->identifier = currentvar;

				switch (obj->v->storage_class)
				{
					case EXTERN:
						fixup->value.type = EXTERN;
						fixup->value.val.identifier = strdup(obj->v->identifier);
						break;

					case STATIC:
						fixup->value.type = STATIC;
						fixup->value.val.number = obj->v->offset;
						break;

					default:
						ierror(0);
				}
				fixup->value.offset = 0;
				fixup->offset = obj->val.vlong;
				fprintf(fp, " (0) (0)\n");
				currentvar.offset += 2;
			}
			break;

		default:
			printf("type %d\n", typf);
			ierror(0);
	}
}

/* Returns the offset of the (STATIC or AUTO) given object. */

zmax voff(struct obj* obj)
{
	zmax offset = obj->v->offset;
	if (offset < 0)
		offset = stackparamadjust + stackoffset - offset - maxalign;
	else
		offset += stackparamadjust;

	offset += obj->val.vlong;
	return offset;
}

/* When a varargs function is called, we need to find where the parameters are
 * on the stack in order to make the __va_start magic variable work. This
 * function does that. */

static int find_varargs(void)
{
	int offset = 0;
	struct reg_handle rh = empty_reg_handle;
	struct struct_declaration* sd = function->vtyp->exact;
	int stackalign;
	int i;

	for (i=0; i<sd->count; i++)
	{
		/* Ignore the parameter if it's been assigned a register. */

		if ((*sd->sl)[i].reg != 0)
			continue;

		/* void shouldn't happen. */

		if (((*sd->sl)[i].styp->flags & NQ) == VOID)
			ierror(0);

		/* Does the backend want to assign it to a register? */

		if (reg_parm(&rh, (*sd->sl)[i].styp, 0, 0))
			continue;

		/* Add on the size of this parameter. */

		offset += sizetab[(*sd->sl)[i].styp->flags & NQ];

		/* Stack align. */

		stackalign = align[(*sd->sl)[i].styp->flags & NQ];
		offset = ((offset+1) / stackalign) * stackalign;
	}

	return (offset + stackoffset);
}

/* Output the name of a global. */

static void emit_identifier(FILE* fp, struct obj* obj)
{
	switch (obj->v->storage_class)
	{
		case STATIC:
			fprintf(fp, "STATIC_%s_%ld",
				modulename, obj->v->offset);
			break;

		case EXTERN:
			fprintf(fp, "_%s", obj->v->identifier);
			break;

		default:
			ierror(0);
	}
}

/* Save a register. */

static void write_reg(FILE* fp, struct obj* obj, int typf, int reg)
{
	int flags = obj->flags &
		(KONST|REG|VAR|DREFOBJ|VARADR);

	/* Constant? */

	if (flags == KONST)
		ierror(0);
		
	/* Dereference? */

	if (flags & DREFOBJ)
		goto dereference;

	/* Register? */

	if ((flags == REG) ||
	    ((flags & VAR) && (flags & REG) && (obj->v->storage_class == AUTO)) ||
	    ((flags & VAR) && (flags & REG) && (obj->v->storage_class == REGISTER)))
	{
		if (flags & DREFOBJ)
			fprintf(fp, "\t@store%c %s 0 %s;\n",
				((typf & NQ) == CHAR) ? 'b' : 'w',
				regnames[obj->reg], regnames[reg]);
		else
		{
			struct zop in;
			struct zop out;
			in.type = ZOP_REG;
			in.val.reg = reg;
			out.type = ZOP_REG;
			out.val.reg = obj->reg;
			emit_add(fp, &in, &zop_zero, &out);
		}
#if 0
			fprintf(fp, "\t@add %s 0 -> %s;\n",
				regnames[reg], regnames[obj->reg]);
#endif
		return;
	}

	/* It must be a variable. */

	switch (obj->v->storage_class)
	{
		case AUTO:
		case REGISTER: /* Local variable */
		{
			zmax offset = voff(obj);

			if ((typf & NQ) == CHAR)
				fprintf(fp, "\t@storeb xp 0%+ld %s;\n",
					offset, regnames[reg]);
			else
			{
				if (offset & 1)
				{
					struct zop c;
					c.type = ZOP_CONSTANT;
					c.val.constant = offset;
					emit_add(fp, &zop_xp, &c, &zop_stack);
					//fprintf(fp, "\t@add xp 0%+ld -> sp;\n", offset);
					fprintf(fp, "\t@storew sp 0 %s;\n", regnames[reg]);
				}
				else
					fprintf(fp, "\t@storew xp 0%+ld %s;\n",
						offset >> 1, regnames[reg]);
			}
			return;
		}

		case EXTERN:
		case STATIC:

			/* Dereference object. */

			if ((typf & NQ) == CHAR)
			{
				fprintf(fp, "\t@storeb ");
				emit_identifier(fp, obj);
				fprintf(fp, " 0%+ld %s;\n",
					obj->val.vlong, regnames[reg]);
			}
			else
			{
				if (obj->val.vlong & 1)
				{
					fprintf(fp, "\t@add ");
					emit_identifier(fp, obj);
					fprintf(fp, " 0%+ld -> sp;\n",
						obj->val.vlong);
					fprintf(fp, "\t@storew sp 0 %s;\n",
						regnames[reg]);
				}
				else
				{
					fprintf(fp, "\t@storew ");
					emit_identifier(fp, obj);
					fprintf(fp, " 0%+ld %s;\n",
						obj->val.vlong >> 1, regnames[reg]);
				}
			}
			return;
#if 0
		case EXTERN: /* External linkage */
			if ((typf & NQ) == CHAR)
				fprintf(fp, "\t@storeb _%s 0%+ld %s;\n",
					obj->v->identifier, offset, regnames[reg]);
			else
			{

				fprintf(fp, "\t@storew _%s 0 %s;\n",
					obj->v->identifier, regnames[reg]);
			return;

		case STATIC: /* Static global */
			if ((typf & NQ) == CHAR)
				fprintf(fp, "\t@storeb STATIC_%s_%ld 0%+ld %s;\n",
					modulename, obj->v->offset, offset, regnames[reg]);
			else
				fprintf(fp, "\t@storew STATIC_%s_%ld 0 %s;\n",
					modulename, obj->v->offset, regnames[reg]);
			return;
#endif

		default:
			ierror(0);
	}

	ierror(0); // Not reached
dereference:
	/* These are a *pain*.
	 *
	 * The first thing we need to do is to read the old contents of the
	 * memory cell, to work out the address we need to write to; and then
	 * do the write. Hurray for the Z-machine stack. */

	obj->flags &= ~DREFOBJ;
	read_reg(fp, obj, POINTER, 0);
	fprintf(fp, "\t@store%c sp 0 %s;\n",
		((typf & NQ) == CHAR) ? 'b' : 'w',
		regnames[reg]);
}

/* Move one register to another register. */

static void move_reg(FILE* fp, int reg1, int reg2)
{
	struct zop r1;
	struct zop r2;
	r1.type = ZOP_REG;
	r1.val.reg = reg1;
	r2.type = ZOP_REG;
	r2.val.reg = reg2;
	emit_add(fp, &r1, &zop_zero, &r2);
}
/* Load a value into a zop. */

static void read_reg(FILE* fp, struct obj* obj, int typf, int reg)
{
	int flags = obj->flags &
		(KONST|REG|VAR|DREFOBJ|VARADR);

	/* The only thing you can do with a function is to take the address of
	 * it. */

	if ((typf & NQ) == FUNKT)
		flags &= ~DREFOBJ & ~VARADR;

	/* Is this a memory dereference? */

	if (flags & DREFOBJ)
		goto dereference;
	
	/* Constant? */

	if (flags == KONST)
	{
		struct zop c;
		struct zop r;
		c.type = ZOP_CONSTANT;
		//fprintf(fp, "\t@add ");
		switch (typf & NQ)
		{
			case CHAR:		c.val.constant = obj->val.vchar;	break;
			case UNSIGNED|CHAR:	c.val.constant = obj->val.vuchar;	break;
			case SHORT:		c.val.constant = obj->val.vshort;	break;
			case UNSIGNED|SHORT:	c.val.constant = obj->val.vushort;	break;
		case POINTER:	      ierror(0);
			case INT:		c.val.constant = obj->val.vint;		break;
			case UNSIGNED|INT:	c.val.constant = obj->val.vuint;	break;
			default:
				ierror(typf);
		}
		r.type = ZOP_REG;
		r.val.reg = reg;
		emit_add(fp, &c, &zop_zero, &r);
		//fprintf(fp, " 0 -> %s;\n", regnames[reg]);
	}
	else if (flags == REG) /* Register? */
	{
		move_reg(fp, obj->reg, reg);
		//fprintf(fp, "\t@add %s 0 -> %s;\n", regnames[obj->reg], regnames[reg]);
	}
	else if ((flags & REG) && ((typf & NQ) == FUNKT)) /* Function pointer? */
	{
		move_reg(fp, obj->reg, reg);
		//fprintf(fp, "\t@add %s 0 -> %s;\n", regnames[obj->reg], regnames[reg]);
	}
	else
	{
		/* It must be a variable. */

		switch (obj->v->storage_class)
		{
			case AUTO:
			case REGISTER: /* Local variable */
				if (flags & VARADR)
				{
					fprintf(fp, "\t@add xp 0%+ld -> %s;\n",
						voff(obj), regnames[reg]);
				}
				else if (flags & REG)
				{
					move_reg(fp, obj->reg, reg);
					//fprintf(fp, "\t@add %s 0 -> %s;\n",
					//	regnames[obj->reg], regnames[reg]);
				}
				else
				{
					zmax offset = voff(obj);

					if ((typf & NQ) == CHAR)
						fprintf(fp, "\t@loadb xp 0%+ld -> %s;\n",
							offset, regnames[reg]);
					else
					{
						if (offset & 1)
						{
							fprintf(fp, "\t@add xp 0%+ld -> sp;\n", offset);
							fprintf(fp, "\t@loadw sp 0 -> %s;\n", regnames[reg]);
						}
						else
							fprintf(fp, "\t@loadw xp 0%+ld -> %s;\n",
								offset >> 1, regnames[reg]);
					}
				}
				break;

			case STATIC:
			case EXTERN: /* Global variable. Implicit dereference,
					with the offset in obj->val.vlong. */

				/* ...but functions are never dereferenced. */

				if ((flags & VARADR) ||
				    ((typf & NQ) == FUNKT))
				{
					/* Fetch address of object. */

					fprintf(fp, "\t@add ");
					emit_identifier(fp, obj);
					fprintf(fp, " 0%+ld -> %s;\n",
						obj->val.vlong, regnames[reg]);
				}
				else if (strcmp(obj->v->identifier, "__va_start") == 0)
				{
					fprintf(fp, "\t@add xp 0%+ld -> %s;\n",
						find_varargs(), regnames[reg]);
				}
				else
				{
					/* Dereference object. */

					if ((typf & NQ) == CHAR)
					{
						fprintf(fp, "\t@loadb ");
						emit_identifier(fp, obj);
						fprintf(fp, " 0%+ld -> %s;\n",
							obj->val.vlong, regnames[reg]);
					}
					else
					{
						if (obj->val.vlong & 1)
						{
							fprintf(fp, "\t@add ");
							emit_identifier(fp, obj);
							fprintf(fp, " 0%+ld -> sp;\n",
								obj->val.vlong);
							fprintf(fp, "\t@loadw sp 0 -> %s;\n",
								regnames[reg]);
						}
						else
						{
							fprintf(fp, "\t@loadw ");
							emit_identifier(fp, obj);
							fprintf(fp, " 0%+ld -> %s;\n",
								obj->val.vlong >> 1, regnames[reg]);
						}
					}
				}
				break;

			default:
				ierror(obj->v->storage_class);
		}
	}
	return;

dereference:
	/* Do we need to dereference the thing we just fetched? */

	/* Fetch the value to dereference. */
	obj->flags &= ~DREFOBJ;
	read_reg(fp, obj, POINTER, 0);
		
	if (flags & DREFOBJ)
	{
		switch (typf & NQ)
		{
			case CHAR:
				fprintf(fp, "\t@loadb sp 0 -> %s;\n",
					regnames[reg], regnames[reg]);
				break;

			case SHORT:
			case INT:
			case POINTER:
			case FUNKT:
				fprintf(fp, "\t@loadw sp 0 -> %s;\n",
					regnames[reg], regnames[reg]);
				break;

			default:
				ierror(typf & NQ);
		}
	}
}

/* Returns the zop to use for an input parameter, pushing that parameter onto
 * the stack if necessary. */

static void push_value(FILE* fp, struct obj* obj, int typf, struct zop* op)
{
	int flags = obj->flags &
		(KONST|REG|VAR|DREFOBJ|VARADR);

	if (flags == KONST)
	{
		op->type = ZOP_CONSTANT;
		switch (typf & NU)
		{
			case CHAR:		op->val.constant = obj->val.vchar;	break;
			case UNSIGNED|CHAR:	op->val.constant = obj->val.vuchar;	break;
			case SHORT:		op->val.constant = obj->val.vshort;	break;
			case UNSIGNED|SHORT:	op->val.constant = obj->val.vushort;	break;
			case INT:		op->val.constant = obj->val.vint;	break;
			case UNSIGNED|INT:	op->val.constant = obj->val.vuint;	break;
		case POINTER:		ierror(0);
			default:
				fprintf(fp, "XXX !!! bad konst type %X\n", typf);
		}
		return;
	}

	/* The only thing you can do with a function is to take the address of it. */

	if ((typf & NQ) == FUNKT)
		flags &= ~DREFOBJ & ~VARADR;

	/* This is used by the long code. The longop functions can only operate
	 * on pointers to longs; so if we need to pass in a constant, we have
	 * to stash it on the stack and return a pointer. */

	if (flags == (KONST|VARADR))
	{
		op->type = ZOP_CONSTANTADDR;
		op->val.constant = addconstant(obj->val.vlong);
		return;
	}
	
	if (flags == REG)
	{
		debugemit(fp, "! zop reg %d\n", obj->reg);
		op->type = ZOP_REG;
		op->val.reg = obj->reg;
		return;
	}

	if ((flags == (VAR|REG)) &&
	    ((obj->v->storage_class == AUTO) ||
	     (obj->v->storage_class == REGISTER)))
	{
		debugemit(fp, "! zop var reg %d\n", obj->reg);
		op->type = ZOP_REG;
		op->val.reg = obj->reg;
		return;
	}

	if ((flags == (VAR|VARADR)) &&
	    (obj->v->storage_class == EXTERN) &&
	    (obj->v->offset == 0))
	{
		debugemit(fp, "! zop varaddr extern %s\n", obj->v->identifier);
		op->type = ZOP_EXTERN;
		op->val.identifier = obj->v->identifier;
		return;
	}

	if ((flags == (VAR|VARADR)) &&
	    (obj->v->storage_class == STATIC) &&
	    (obj->v->offset == 0))
	{
		debugemit(fp, "! zop varaddr static %ld\n", obj->v->offset);
		op->type = ZOP_STATIC;
		op->val.constant = obj->v->offset;
		return;
	}

	if ((flags & VAR) &&
	    ((obj->v->vtyp->flags & NQ) == FUNKT))
	{
		if (obj->v->storage_class == EXTERN)
		{
			op->type = ZOP_EXTERN;
			op->val.identifier = obj->v->identifier;
		}
		else
		{
			op->type = ZOP_STATIC;
			op->val.constant = obj->v->offset;
		}
		return;
	}
	
	read_reg(fp, obj, typf, 0);
	op->type = ZOP_STACK;
}

/* Same as push_value(), but returns a zop for the *address* of the object, not
 * the object itself. Used a lot by the long code. */

static void push_addrof(FILE* fp, struct obj* obj, int typf, struct zop* op)
{
	if (obj->flags & DREFOBJ)
		obj->flags &= ~DREFOBJ;
	else
		obj->flags |= VARADR;
	push_value(fp, obj, POINTER, op);
}

/* Returns the zop to use for an output parameter. Unlike push_value, this does
 * not emit a pop; that must be done later, if the return parameter is zero. */

static void pop_value(FILE* fp, struct obj* obj, int typf, struct zop* op)
{
	int flags = obj->flags &
		(KONST|REG|VAR|DREFOBJ|VARADR);

	/* We don't even *try* to handle dereferences here. */

	if (flags & DREFOBJ)
		goto stack;

	if (flags == REG)
		goto reg;

	if ((flags == (VAR|REG)) &&
	    ((obj->v->storage_class == AUTO) ||
	     (obj->v->storage_class == REGISTER)))
		goto reg;

stack:
	op->type = ZOP_STACK;
	return;

reg:
	op->type = ZOP_REG;
	op->val.reg = obj->reg;
}

/* Writes code for a zop. */

static void emit_zop(FILE* fp, struct zop* op)
{
	switch (op->type)
	{
		case ZOP_STACK:
			fprintf(fp, "sp");
			return;

		case ZOP_REG:
			fprintf(fp, "%s", regnames[op->val.reg]);
			return;

		case ZOP_CONSTANT:
			fprintf(fp, "0%+ld", (zshort)op->val.constant);
			return;

		case ZOP_EXTERN:
			fprintf(fp, "_%s", op->val.identifier);
			return;

		case ZOP_STATIC:
			fprintf(fp, "STATIC_%s_%ld",
				modulename, op->val.constant);
			return;

		case ZOP_CONSTANTADDR:
			fprintf(fp, "CONSTANT_%s_%ld",
				modulename, op->val.constant);
			return;

		default:
			ierror(op->type);
	}
}

/* This is used in conjunction with pop_value(). pop_value() returns a zop that
 * represents the return value for a function. If that return value is the
 * stack, the value on the stack needs to be written back into memory. That's
 * what this function does. */

static void fin_zop(FILE* fp, struct obj* obj, int typf, struct zop* op)
{
	switch (op->type)
	{
		case ZOP_STACK:
			write_reg(fp, obj, typf, 0);
			return;

		case ZOP_REG:
			return;

		default:
			ierror(0);
	}
}

/* Emit a basic ADD instruction.
 * This routine tests for all the various special cases, of which there are
 * many, and attempts to produce optimal code.
 */

static void emit_add(FILE* fp, struct zop* q1, struct zop* q2, struct zop* z)
{
	/* Sometimes we get ZOP_REG with reg=0. This actually means the stack. */

	if ((q1->type == ZOP_REG) && (q1->val.reg == 0))
		q1 = &zop_stack;
	if ((q2->type == ZOP_REG) && (q2->val.reg == 0))
		q2 = &zop_stack;
	if ((z->type == ZOP_REG) && (z->val.reg == 0))
		z = &zop_stack;

	/* If q2 is a constant and 0, then this might be a register move of
	 * some kind. */

	if ((q2->type == ZOP_CONSTANT) && (q2->val.constant == 0))
	{
		/* Left is a register? */
		if (q1->type == ZOP_REG)
		{
			/* Right is a register? */
			if (z->type == ZOP_REG)
			{
				/* They're the *same* register? */
				if (q1->val.reg == z->val.reg)
				{
					/* No code need be emitted. */
					return;
				}

				/* Emit a @store instruction. Unfortunately, I
				 * can't work out the syntax for Inform's
				 * @store opcode, so we emit a high-level
				 * assignment instead and let Inform work it
				 * out. */

				fprintf(fp, "\t");
				emit_zop(fp, z);
				fprintf(fp, " = ");
				emit_zop(fp, q1);
				fprintf(fp, ";\n");
				return;
			}

			/* Right is the stack? */
			if (z->type == ZOP_STACK)
			{
				/* We're pushing the single parameter onto the
				 * stack. */

				fprintf(fp, "\t@push ");
				emit_zop(fp, q1);
				fprintf(fp, ";\n");
				return;
			}
		}

		/* Left is the stack? */
		if (q1->type == ZOP_STACK)
		{
			/* Right is a register? */
			if (z->type == ZOP_REG)
			{
				/* We're popping the single parameter off the
				 * stack. */

				fprintf(fp, "\t@pull ");
				emit_zop(fp, z);
				fprintf(fp, ";\n");
				return;
			}
		}
	}

	/* Fall back on an ordinary @add. */

	fprintf(fp, "\t@add ");
	emit_zop(fp, q1);
	fprintf(fp, " ");
	emit_zop(fp, q2);
	fprintf(fp, " -> ");
	emit_zop(fp, z);
	fprintf(fp, ";\n");
}

/* Copy a value from one zop to another. This is not quite as simple as you
 * might think, because there are a number of optimisation cases to take into
 * account.
 *
 * NOTE: for simplicity, this function will never emit just a single
 * instruction --- the assignment is always done via the stack. FIXME. */

static void move_value(FILE* fp, struct obj* q1o, struct obj* zo, int typf)
{
	struct zop q1;
	struct zop z;

	pop_value(fp, zo, typf, &z);
	push_value(fp, q1o, typf, &q1);
	debugemit(fp, "! L=%d R=%d\n", q1.type, z.type);
	/* In all cases except when push_value() and fin_zop() *both* emit
	 * code, we need to insert an assignment here. As they only emit code
	 * in the ZOP_STACK case... */
	if ((q1.type != ZOP_STACK) || (z.type != ZOP_STACK))
	{
		emit_add(fp, &q1, &zop_zero, &z);
#if 0
		fprintf(fp, "\t@add ");
		emit_zop(fp, &q1);
		fprintf(fp, " 0 -> ");
		emit_zop(fp, &z);
		fprintf(fp, ";\n");
#endif
	}
	fin_zop(fp, zo, typf, &z);
}
	
/* Copy a 32-bit value from one obj to another. */

static void move_long_value(FILE* fp, struct obj* q1, struct obj* z, int typf)
{
	int flags = q1->flags &
		(KONST|REG|VAR|DREFOBJ|VARADR);
	struct zop q1z;
	struct zop zz;
	
	if (flags == KONST)
	{
		int hi = xword(q1->val.vlong, 1);
		int lo = xword(q1->val.vlong, 0);

		push_addrof(fp, z, POINTER, &zz);
		fprintf(fp, "\t@call_vn __long_loadconst ");
		emit_zop(fp, &zz);
		fprintf(fp, " 0%+ld 0%+ld;\n", (short)hi, (short)lo);
		return;
	}

	push_addrof(fp, z, POINTER, &zz);
	push_addrof(fp, q1, POINTER, &q1z);
	fprintf(fp, "\t@copy_table ");
	emit_zop(fp, &q1z);
	fprintf(fp, " ");
	emit_zop(fp, &zz);
	fprintf(fp, " 4;\n");
}

/* The code generator itself.
 * This big, complicated, hairy and scary function does the work to actually
 * produce the code.  fp is the output stream, ic the beginning of the ic
 * chain, func is a pointer to the actual function and stackframe is the size
 * of the function's stack frame.
 */

void gen_code(FILE* fp, struct IC *ic, struct Var* func, zmax stackframe)
{
	int i;
	struct zop q1;
	struct zop q2;
	struct zop z;
	int code, typf; // ...of the IC under consideration

    int c,t,lastcomp=0,reg;
	    
    	function = func;

	/* r0..r5 are always used for parameter passing. */

	regused[2] = 1;
	regused[3] = 1;
	regused[4] = 1;
	regused[5] = 1;
	regused[6] = 1;
	regused[7] = 1;

	/* This is the offset of the stack frame, relative to the current stack
	 * pointer. */

	stackoffset = stackframe;

	/* No parameters pushed yet. */

	stackparamadjust = 0;

	reflower(fp);

	if (func->storage_class == STATIC)
		fprintf(fp, "[ STATIC_%s_%ld xp\n", modulename, func->offset);
	else
		fprintf(fp, "[ _%s xp\n", func->identifier);

	/* Tell Inform what registers the function is using. */

	for (i=1; i<=MAXR; i++)
	{
		//fprintf(fp, "! i=%d used %d scratch %d alloc %d\n",
		//		i, regused[i], regscratch[i], regsa[i]);
		if (regused[i] && !regsa[i])
			fprintf(fp, "\t%s\n", regnames[i]);
	}
	fprintf(fp, ";\n");

	/* Trace the function name. */

	if (g_flags[1] & USEDFLAG)
	{
		if (func->storage_class == STATIC)
			fprintf(fp, "print \"STATIC_%s_%ld^\";\n", modulename, func->offset);
		else
			fprintf(fp, "print \"_%s^\";\n", func->identifier);
	}
	
	/* Adjust stack for locals. */

	if (stackframe)
		fprintf(fp, "\t@sub xp 0%+ld -> xp;\n", stackframe);
	//if (stackoffset)
	//	fprintf(fp, "\txp = xp - %ld\n", stackframe);

    
    	/* Iterate through all ICs. */

	for (; dump_ic(fp, ic), ic; ic=ic->next)
	{
        c=ic->code;t=ic->typf;
		code = ic->code;
		typf = ic->typf;

		/* Do nothing for NOPs. */

		if (code == NOP)
			continue;

		/* Has the stack been adjusted due to a call? */

#if 0
		if (stackcalladjustment)
		{
			if ((code != GETRETURN) &&
			    (code != FREEREG) &&
			    (code != ALLOCREG))
			{
				debugemit(fp, "! stack reset %d %d\n",
					stackparamadjust, stackcallparamsize);
				fprintf(fp, "\t@add xp %d -> xp;\n",
					stackparamadjust+stackcallparamsize);
				stackparamadjust = 0;
				stackcallparamsize = 0;
				stackcalladjustment = 0;
			}
		}
#endif

#if 0
        if(notpopped&&!dontpop){
            int flag=0;
            if(c==LABEL||c==COMPARE||c==TEST||c==BRA){
                fprintf(fp,"\tadd\t%s,#%ld\n",regnames[sp],notpopped);
                stackoffset+=notpopped;notpopped=0;
            }
        }
#endif
		/* These opcodes turn into other opcodes. */

		switch (code)
		{
			case SUBPFP:
			case SUBIFP:
				code = SUB;
				break;

			case ADDI2P:
				code = ADD;
				break;
		}

		/* And now the big opcode switch. */

		switch (code)
		{
			case ALLOCREG: /* Mark register in use */
				regs[ic->q1.reg] = 1;
				continue;

			case FREEREG: /* Mark register not in use */
				regs[ic->q1.reg] = 0;
				continue;

			case LABEL: /* Emit jump target */
				fprintf(fp, ".%s%d;\n",
					labelprefix, typf);
				continue;

			case BRA: /* Unconditional jump */
				fprintf(fp, "\tjump %s%d;\n",
					labelprefix, typf);
				continue;

			case GETRETURN: /* Read the last function call's return parameter */
				switch (typf & NQ)
				{
					case CHAR:
						//if (ic->q2.val.vlong != 1)
						//	goto copy_struct;
						/* fall through */
					case SHORT:
					case INT:
					case POINTER:
						write_reg(fp, &ic->z, typf, 2);
						break;

						/* Ignore the following; the
						 * front-end will automatically
						 * pass in an implicit
						 * parameter to the function
						 * containing the address of
						 * the return parameter, so
						 * GETRETURN ought to be a
						 * noop. */
					case LONG:
					case STRUCT:
					case VOID:
					case ARRAY:
						break;
#if 0
					copy_struct:
						push_addrof(fp, &ic->z, typf, &z);
						fprintf(fp, "\t@copy_table xp ");
						emit_zop(fp, &z);
						fprintf(fp, " %ld;\n", szof(ic->z.v->vtyp));
						break;
#endif

					default:
						ierror(typf & NQ);
				}
				//fprintf(fp, "\tr0 = ");
				//emit_object(fp, &ic->q1, typf);
				//fprintf(fp, ";\n");
				//write_reg(fp, &ic->z, typf, 2);
				continue;

			case SETRETURN: /* Set this function's return parameter */
				switch (typf & NQ)
				{
					case CHAR:
						//if (ic->q2.val.vlong != 1)
						//	goto setreturn_copy_struct;

						/* fall through */
					case SHORT:
					case INT:
					case POINTER:
						read_reg(fp, &ic->q1, typf, 2);
						break;

					case LONG:
					case STRUCT:
					case VOID:
					case ARRAY:
#if 0
					setreturn_copy_struct:
						fprintf(fp, "\t@add xp %ld -> sp;\n",
							stackoffset);
						push_addrof(fp, &ic->q1, typf, &q1);
						fprintf(fp, "\t@copy_table ");
						emit_zop(fp, &q1);
						fprintf(fp, " sp %ld;\n", szof(ic->q1.v->vtyp));
						break;
#endif

					default:
						ierror(typf & NQ);
				}
				//fprintf(fp, "\tr0 = ");
				//emit_object(fp, &ic->q1, typf);
				//fprintf(fp, ";\n");
				continue;

			case MINUS: /* Unary minus */
				switch (typf & NQ)
				{
					case CHAR:
					case SHORT:
					case INT:
						push_value(fp, &ic->q1, typf, &q1);
						pop_value(fp, &ic->z, typf, &z);
						fprintf(fp, "\t@sub 0 ");
						emit_zop(fp, &q1);
						fprintf(fp, " -> ");
						emit_zop(fp, &z);
						fprintf(fp, ";\n");
						fin_zop(fp, &ic->z, typf, &z);
						break;

					case LONG:
						push_addrof(fp, &ic->z, typf, &z);
						push_addrof(fp, &ic->q1, typf, &q1);
						fprintf(fp, "\t@call_vn __long_neg ");
						emit_zop(fp, &q1);
						fprintf(fp, " ");
						emit_zop(fp, &z);
						fprintf(fp, ";\n");
						break;

					default:
						ierror(0);
				}
				continue;

			case KOMPLEMENT: /* Unary komplement */
				/* INFORM BUG! */
				/* The @not opcode doesn't work. We have to use a
				 * wrapper function instead. */
				
				push_value(fp, &ic->q1, typf, &q1);
				pop_value(fp, &ic->z, typf, &z);
				fprintf(fp, "\t@call_2s __not ");
				emit_zop(fp, &q1);
				fprintf(fp, " -> ");
				emit_zop(fp, &z);
				fprintf(fp, ";\n");
				fin_zop(fp, &ic->z, typf, &z);
				continue;

			case MOVEFROMREG: /* Write a register to memory */
				write_reg(fp, &ic->z, typf, ic->q1.reg);
				continue;

			case MOVETOREG: /* Read a register from memory */
				read_reg(fp, &ic->q1, typf, ic->z.reg);
				continue;

			case ASSIGN: /* Move something to somewhere else */
				debugemit(fp, "! ASSIGN size %d typf %d\n", ic->q2.val.vlong, typf & NQ);
				switch (typf & NQ)
				{
					case CHAR:
						if (ic->q2.val.vlong != 1)
							goto assign_copy_struct;
						/* fall through */
					case SHORT:
					case INT:
					case POINTER:
						move_value(fp, &ic->q1, &ic->z, typf);
						break;

					case LONG:
						move_long_value(fp, &ic->q1, &ic->z, typf);
						break;

					case STRUCT:
					case VOID:
					case ARRAY:
					assign_copy_struct:
						push_addrof(fp, &ic->z, typf, &z);
						push_addrof(fp, &ic->q1, typf, &q1);
						fprintf(fp, "\t@copy_table ");
						emit_zop(fp, &q1);
						fprintf(fp, " ");
						emit_zop(fp, &z);
						fprintf(fp, " 0%+ld;\n", ic->q2.val.vlong);
						break;

					default:
						ierror(typf & NQ);
				}
				continue;

			case ADDRESS: /* Fetch the address of something, always
					 AUTO or STATIC */
				i = voff(&ic->q1);
				pop_value(fp, &ic->z, typf, &z);
				fprintf(fp, "\t@add xp 0%+ld -> ", i);
				emit_zop(fp, &z);
				fprintf(fp, ";\n");
				fin_zop(fp, &ic->z, typf, &z);
				continue;

			case PUSH: /* Push a value onto the stack */
				fprintf(fp, "\t@sub xp 0%+ld -> xp;\n",
					ic->q2.val.vlong);
				//stackoffset += ic->q2.val.vlong;
				stackparamadjust += ic->q2.val.vlong;

				switch (ic->q2.val.vlong)
				{
					case 1:
						push_value(fp, &ic->q1, typf, &q1);
						fprintf(fp, "\t@storeb xp 0 ");
						emit_zop(fp, &q1);
						fprintf(fp, ";\n");
						break;

					case 2:
						push_value(fp, &ic->q1, typf, &q1);
						fprintf(fp, "\t@storew xp 0 ");
						emit_zop(fp, &q1);
						fprintf(fp, ";\n");
						break;

					default:
						push_addrof(fp, &ic->q1, typf, &q1);
						fprintf(fp, "\t@copy_table ");
						emit_zop(fp, &q1);
						fprintf(fp, " xp 0%+ld;\n", ic->q2.val.vlong);
						break;
				}
				continue;

			case ADD: /* Add two numbers */
			case SUB: /* Subtract two numbers */
			case MULT: /* Multiply two numbers */
			case DIV: /* Divide two numbers */
			case MOD: /* Modulo two numbers */
			case OR: /* Bitwise or */
			case XOR: /* Bitwise xor */
			case AND: /* Bitwise and */
			case LSHIFT: /* Shift left */
			case RSHIFT: /* Shift right */
				switch (typf & NQ)
				{
					case CHAR:
					case SHORT:
					case INT:
					case POINTER:
						/* Second parameter first! */
						push_value(fp, &ic->q2, typf, &q2);

						if (code == RSHIFT)
						{
							fprintf(fp, "\t@sub 0 ");
							emit_zop(fp, &q2);
							fprintf(fp, " -> sp;\n");
							q2.type = ZOP_STACK;
						}

						push_value(fp, &ic->q1, typf, &q1);
						pop_value(fp, &ic->z, typf, &z);
						//fprintf(fp, "\t");
						//emit_object(fp, &ic->z, typf);
						//fprintf(fp, " = ");
						//emit_object(fp, &ic->q1, typf);
						switch (code)
						{
							case ADD:
								fprintf(fp, "\t@add ");
								break;

							case SUB:
								fprintf(fp, "\t@sub ");
								break;

							case MULT:
								fprintf(fp, "\t@mul ");
								break;

							case DIV:
								if (typf & UNSIGNED)
									fprintf(fp, "\t@call_vs __unsigned_div ");
								else
									fprintf(fp, "\t@div ");
								break;

							case MOD:
								if (typf & UNSIGNED)
									fprintf(fp, "\t@call_vs __unsigned_mod ");
								else
									fprintf(fp, "\t@mod ");
								break;

							case AND:
								fprintf(fp, "\t@and ");
								break;

							case XOR:
								fprintf(fp, "\t@call_vs __xor ");
								break;

							case OR:
								fprintf(fp, "\t@or ");
								break;

							case LSHIFT:
							case RSHIFT:
								if (typf & UNSIGNED)
									fprintf(fp, "\t@log_shift ");
								else
									fprintf(fp, "\t@art_shift ");
								break;

							default:
								/* Should never get here! */
								ierror(0);
						}
						emit_zop(fp, &q1);
						fprintf(fp, " ");
						emit_zop(fp, &q2);
						fprintf(fp, " -> ");
						emit_zop(fp, &z);
						fprintf(fp, ";\n");
						fin_zop(fp, &ic->z, typf, &z);
						//emit_object(fp, &ic->q2, typf);
						break;

					case LONG:
						/* Destination parameter first! */

						push_addrof(fp, &ic->z, typf, &z);
						push_addrof(fp, &ic->q2, typf, &q2);
						push_addrof(fp, &ic->q1, typf, &q1);

						fprintf(fp, "\t@call_vn __long_");
						switch (code)
						{
							case ADD:
								fprintf(fp, "add");
								break;

							case SUB:
								fprintf(fp, "sub");
								break;

							case MULT:
								fprintf(fp, "mul");
								break;

							case DIV:
								if (typf & UNSIGNED)
									fprintf(fp, "unsigned_div");
								else
									fprintf(fp, "div");
								break;

							case MOD:
								if (typf & UNSIGNED)
									fprintf(fp, "unsigned_mod");
								else
									fprintf(fp, "mod");
								break;

							case AND:
								fprintf(fp, "and");
								break;

							case XOR:
								fprintf(fp, "xor");
								break;

							case OR:
								fprintf(fp, "or");
								break;

							case LSHIFT:
								fprintf(fp, "lsl");
								break;
								
							case RSHIFT:
								if (typf & UNSIGNED)
									fprintf(fp, "lsr");
								else
									fprintf(fp, "asr");
								break;

							default:
								/* Should never get here! */
								ierror(0);
						}
						fprintf(fp, " ");
						emit_zop(fp, &q1);
						fprintf(fp, " ");
						emit_zop(fp, &q2);
						fprintf(fp, " ");
						emit_zop(fp, &z);
						fprintf(fp, ";\n");
						break;

					default:
						ierror(0);
				}
				continue;

		case CONVERT:
		  if((q1typ(ic)&NU)==CHAR){
				switch (ztyp(ic) & NU)
				{
					case CHAR:
					case UNSIGNED|CHAR:
					case UNSIGNED|SHORT:
					case UNSIGNED|INT:
					case SHORT:
					case INT:
						push_value(fp, &ic->q1, CHAR, &q1);
						pop_value(fp, &ic->z, typf, &z);
						fprintf(fp, "\t@log_shift ");
						emit_zop(fp, &q1);
						fprintf(fp, " 8 -> sp;\n");
						fprintf(fp, "\t@art_shift sp 0-8 -> ");
						emit_zop(fp, &z);
						fprintf(fp, ";\n");
						fin_zop(fp, &ic->z, typf, &z);
						break;
					
					case LONG:
						push_value(fp, &ic->q1, INT, &q1);
						push_addrof(fp, &ic->z, typf, &z);
						fprintf(fp, "\t@call_vn __long_fromchar");
						emit_zop(fp, &z);
						fprintf(fp, " ");
						emit_zop(fp, &q1);
						fprintf(fp, ";\n");
						break;

					default:
						ierror(0);
				}
				continue;
		  }
		  if((q1typ(ic)&NU)==(UNSIGNED|CHAR)){

				switch (ztyp(ic) & NQ)
				{
					case CHAR:
					case SHORT:
					case INT:
						push_value(fp, &ic->q1, UNSIGNED|CHAR, &q1);
						pop_value(fp, &ic->z, typf, &z);
						if ((z.type != ZOP_STACK) || (q1.type != ZOP_STACK))
						{
							emit_add(fp, &q1, &zop_zero, &z);
#if 0
							fprintf(fp, "\t@add ");
							emit_zop(fp, &q1);
							fprintf(fp, " 0 -> ");
							emit_zop(fp, &z);
							fprintf(fp, ";\n");
#endif
						}
						fin_zop(fp, &ic->z, typf, &z);
						break;
					
					case LONG:
						push_value(fp, &ic->q1, UNSIGNED|CHAR, &q1);
						push_addrof(fp, &ic->z, typf, &z);
						fprintf(fp, "\t@call_vn __long_fromint");
						emit_zop(fp, &z);
						fprintf(fp, " 0 ");
						emit_zop(fp, &q1);
						fprintf(fp, ";\n");
						break;
						
					default:
						ierror(0);
				}
				continue;
		  }
		  if((q1typ(ic)&NU)==SHORT||(q1typ(ic)&NU)==INT){
				switch (ztyp(ic) & NU)
				{
					case CHAR:
					case UNSIGNED|CHAR:
					case UNSIGNED|SHORT:
					case UNSIGNED|INT:
					case SHORT:
					case INT:
						push_value(fp, &ic->q1, INT, &q1);
						pop_value(fp, &ic->z, typf, &z);
						if ((z.type != ZOP_STACK) || (q1.type != ZOP_STACK))
						{
							emit_add(fp, &q1, &zop_zero, &z);
#if 0
							fprintf(fp, "\t@add ");
							emit_zop(fp, &q1);
							fprintf(fp, " 0 -> ");
							emit_zop(fp, &z);
							fprintf(fp, ";\n");
#endif
						}
						fin_zop(fp, &ic->z, typf, &z);
						break;

					case LONG:
						push_value(fp, &ic->q1, INT, &q1);
						push_addrof(fp, &ic->z, typf, &z);
						fprintf(fp, "\t@call_vn __long_fromint ");
						emit_zop(fp, &z);
						fprintf(fp, " ");
						emit_zop(fp, &q1);
						fprintf(fp, ";\n");
						break;

					case UNSIGNED|LONG:
						push_value(fp, &ic->q1, INT, &q1);
						push_addrof(fp, &ic->z, typf, &z);
						fprintf(fp, "\t@call_vn __long_loadconst ");
						emit_zop(fp, &z);
						fprintf(fp, " 0 ");
						emit_zop(fp, &q1);
						fprintf(fp, ";\n");
						break;
					
					default:
						ierror(typf);
				}
				continue;
		}
		if((q1typ(ic)&NU)==(UNSIGNED|SHORT)||(q1typ(ic)&NU)==(UNSIGNED|INT)||(q1typ(ic)&NU)==POINTER){

				switch (ztyp(ic) & NQ)
				{
					case CHAR:
					case SHORT:
					case INT:
						push_value(fp, &ic->q1, INT, &q1);
						pop_value(fp, &ic->z, typf, &z);
						if ((z.type != ZOP_STACK) || (q1.type != ZOP_STACK))
						{
							emit_add(fp, &q1, &zop_zero, &z);
#if 0
							fprintf(fp, "\t@add ");
							emit_zop(fp, &q1);
							fprintf(fp, " 0 -> ");
							emit_zop(fp, &z);
							fprintf(fp, ";\n");
#endif
						}
						fin_zop(fp, &ic->z, typf, &z);
						break;

					case LONG:
						push_value(fp, &ic->q1, INT, &q1);
						push_addrof(fp, &ic->z, typf, &z);
						fprintf(fp, "\t@call_vn __long_loadconst ");
						emit_zop(fp, &z);
						fprintf(fp, " 0 ");
						emit_zop(fp, &q1);
						fprintf(fp, ";\n");
						break;
#if 0
					case SHORT:
					case INT:
						fprintf(fp, "\t");
						emit_object(fp, &ic->z, typf);
						fprintf(fp, " = (");
						emit_object(fp, &ic->q1, CHAR);
						fprintf(fp, ") << 8 >> 8;\n");
						break;
#endif
					
					default:
						printf("%X\n", typf);
						ierror(0);
				}
				continue;
		}
		if((q1typ(ic)&NU)==(UNSIGNED|LONG)||(q1typ(ic)&NU)==LONG){

				switch (ztyp(ic) & NQ)
				{
					case CHAR:
						push_addrof(fp, &ic->q1, LONG, &q1);
						pop_value(fp, &ic->z, typf, &z);
						fprintf(fp, "\t@loadb ");
						emit_zop(fp, &q1);
						fprintf(fp, " 3 -> ");
						emit_zop(fp, &z);
						fprintf(fp, ";\n");
						fin_zop(fp, &ic->z, typf, &z);
						break;

					case SHORT:
					case INT:
						push_addrof(fp, &ic->q1, LONG, &q1);
						pop_value(fp, &ic->z, typf, &z);
						fprintf(fp, "\t@loadw ");
						emit_zop(fp, &q1);
						fprintf(fp, " 1 -> ");
						emit_zop(fp, &z);
						fprintf(fp, ";\n");
						fin_zop(fp, &ic->z, typf, &z);
						break;
					
					default:
						ierror(typf & NQ);
				}
				continue;
		}
			case COMPARE:
				/* COMPARE is special. The next instruction is
				 * always a branch.  The Z-machine does
				 * branches in the form:
				 *
				 * @j{e,g,l} <var1> <var2> [~]@<label>
				 *
				 * However, we don't know what short of branch
				 * to emit until the next instruction (which is
				 * the IC for a branch). So we have to stash
				 * the zops that we're using for the
				 * compare here, for use later. This is done

				 * using the globals compare1 and compare2.
				 */

				switch (typf & NU)
				{
					case CHAR:
					case SHORT:
					case INT:
					case POINTER:
						/* Second parameter first! */
						push_value(fp, &ic->q2, typf, &compare2);
						push_value(fp, &ic->q1, typf, &compare1);
						break;

					case UNSIGNED|CHAR:
					case UNSIGNED|SHORT:
					case UNSIGNED|INT:
						/* Because the Z-machine only
						 * has signed comparisons, we
						 * need a dodgy algorithm to
						 * do this, which works as
						 * follows: in the signed
						 * domain, 0-7FFF compares
						 * greater than 8000-FFFF. In
						 * the unsigned domain, it's
						 * the other way around. So,
						 * by flipping the sign bits
						 * we do the logical
						 * equivalent of shifting the
						 * unsigned range up/down by
						 * 8000 which makes it fit
						 * the signed range. There.
						 * Did you understand that?
						 * Neither did I, the first
						 * few times it was explained
						 * to me. */
						read_reg(fp, &ic->q2, typf, 0);
						fprintf(fp, "\t@add sp $8000 -> sp;\n");
						read_reg(fp, &ic->q1, typf, 0);
						fprintf(fp, "\t@add sp $8000 -> sp;\n");
						compare1.type = ZOP_STACK;
						compare2.type = ZOP_STACK;
						break;

					case LONG:
						push_addrof(fp, &ic->q2, typf, &q2);
						push_addrof(fp, &ic->q1, typf, &q1);
						fprintf(fp, "\t@call_vs __long_compare ");
						emit_zop(fp, &q1);
						fprintf(fp, " ");
						emit_zop(fp, &q2);
						fprintf(fp, " -> sp;\n");
						compare1.type = ZOP_STACK;
						compare2.type = ZOP_CONSTANT;
						compare2.val.constant = 0;
						break;

					case UNSIGNED|LONG:
						push_addrof(fp, &ic->q2, typf, &q2);
						push_addrof(fp, &ic->q1, typf, &q1);
						fprintf(fp, "\t@call_vs __long_unsigned_compare ");
						emit_zop(fp, &q1);
						fprintf(fp, " ");
						emit_zop(fp, &q2);
						fprintf(fp, " -> sp;\n");
						compare1.type = ZOP_STACK;
						compare2.type = ZOP_CONSTANT;
						compare2.val.constant = 0;
						break;

					default:
						ierror(typf & NQ);
				}
				continue;

			case TEST:
				/* TEST is a special COMPARE. It takes one
				 * parameter and always tests it against 0; it
				 * is guaranteed to be followed by BNE or BEQ.
				 * */

				switch (typf & NQ)
				{
					case CHAR:
					case SHORT:
					case INT:
					case POINTER:
						push_value(fp, &ic->q1, typf, &compare1);
						compare2.type = ZOP_CONSTANT;
						compare2.val.constant = 0;
						break;

					case LONG:
						push_addrof(fp, &ic->q1, typf, &q1);
						fprintf(fp, "\t@call_vs __long_compare ");
						emit_zop(fp, &q1);
						fprintf(fp, " ");
						q2.type = ZOP_CONSTANTADDR;
						q2.val.constant = addconstant(0);
						emit_zop(fp, &q2);
						fprintf(fp, " -> sp;\n", i);
						compare1.type = ZOP_STACK;
						compare2.type = ZOP_CONSTANT;
						compare2.val.constant = 0;
						break;

					default:
						ierror(typf & NQ);
				}
				continue;

			case BEQ:
			case BNE:
			case BLT:
			case BGE:
			case BLE:
			case BGT:
			{
				static int branchlabel = 0;

				fprintf(fp, "\t@j");
				switch (code)
				{
					case BNE:
					case BEQ:	fprintf(fp, "e ");	break;
					case BLT:
					case BGE:	fprintf(fp, "l ");	break;
					case BGT:
					case BLE:	fprintf(fp, "g ");	break;
				}

				emit_zop(fp, &compare1);
				fprintf(fp, " ");
				emit_zop(fp, &compare2);
				fprintf(fp, " ?");

				if (g_flags[3] & USEDFLAG)
				{
					if (!((code == BNE) || (code == BGE) || (code == BLE)))
						fprintf(fp, "~");
					fprintf(fp, "LL%d;\n", branchlabel);
					fprintf(fp, "\tjump %s%d;\n", labelprefix, typf);
					fprintf(fp, ".LL%d;\n", branchlabel++);
				}
				else
				{
					if ((code == BNE) || (code == BGE) || (code == BLE))
						fprintf(fp, "~");
					fprintf(fp, "%s%d;\n", labelprefix, typf);
				}
				continue;
			}

			case CALL:
			{
#if 0
				/* Calculate the amount of stack to reserve for
				 * the return parameter. ints and smaller go in
				 * the return register. */

				stackcallparamsize = szof(ic->q1.v->vtyp->next);
				if (stackcallparamsize <= sizetab[INT])
					stackcallparamsize = 0;

				if (stackcallparamsize)
					fprintf(fp, "\t@sub xp %d -> xp;\n",
						stackcallparamsize);
#endif

				/* Is this actually an inline assembly function? */

				if ((ic->q1.flags & VAR) &&
				    ic->q1.v->fi &&
				    ic->q1.v->fi->inline_asm)
				{
					/* Yes. Emit the assembly code. */

					fprintf(fp, "%s", ic->q1.v->fi->inline_asm);
				}
				else
				{
					/* No; so emit a call. */
				
					push_value(fp, &ic->q1, typf, &q1);
					fprintf(fp, "\t@call_vs2 ");
					emit_zop(fp, &q1);
					fprintf(fp, " xp r0 r1 r2 r3 r4 r5 -> r0;\n");
				}

				//stackcalladjustment = 1;

				/* If any parameters have been pushed, adjust
				 * the stack to pop them. */

				if (stackparamadjust)
				{
					fprintf(fp, "\t@add xp 0%+ld -> xp;\n",
						stackparamadjust);
					//stackoffset -= stackparamadjust;
					stackparamadjust = 0;
				}
				continue;
			}

			default:
				ierror(code);
		}
						
	}

	/* We really ought to tidy the stack up; but there's no need, because
	 * the old value of xp will be restored when the function exits. */

    	//if (stackframe)
	//	fprintf(fp, "\t@add xp %ld -> xp;\n", stackframe);
	
	fprintf(fp, "\t@ret r0;\n");
	fprintf(fp, "]\n");

//    function_bottom(fp, func, loff);
}

int shortcut(int code, int typ)
{
    return(0);
}

// Add a constant to the constant pool.

static int addconstant(zmax value)
{
	struct constant* c;
	
	/* Check to see if the constant's already in the pool. */

	c = constantlist;
	while (c)
	{
		if (c->value == value)
			return c->id;
		c = c->next;
	}

	/* It's not; add it. */
	
	c = malloc(sizeof(struct constant));
	c->next = constantlist;
	c->id = constantnum++;
	c->value = value;
	constantlist = c;
	return c->id;
}

void cleanup_cg(FILE *fp)
{
	struct fixup* fixup = fixuplist;

	/* Have we actually emitted anything? */

	if (!fp)
		return;

	reflower(fp);

	/* Emit the constant pool. */

	{
		struct constant* constant = constantlist;

		while (constant)
		{
			fprintf(fp, "Array CONSTANT_%s_%ld -->\n",
				modulename, constant->id);
			fprintf(fp, " 0%+ld 0%+ld;\n",
				xword(constant->value, 1),
				xword(constant->value, 0));
			constant = constant->next;
		}
	}

	/* Emit the code to initialise the data area. */

	{
		struct fixup* fixup = fixuplist;

		fprintf(fp, "[ __init_vars_%s;\n", modulename);
		while (fixup)
		{
			fprintf(fp, "\t@add 0%+ld ", fixup->offset);

			switch (fixup->value.type)
			{
				case STATIC:
					fprintf(fp, "STATIC_%s_%ld -> sp;\n",
						modulename, fixup->value.val.number);
					break;

				case EXTERN:
					fprintf(fp, "_%s -> sp;\n",
						fixup->value.val.identifier);
					break;

				default:
					ierror(0);
			}

			switch (fixup->identifier.type)
			{
				case STATIC:
					fprintf(fp, "\t@storew STATIC_%s_%ld 0%+ld sp;\n",
						modulename, fixup->identifier.val.number,
						fixup->identifier.offset);
					break;

				case EXTERN:
					fprintf(fp, "\t@storew _%s 0%+ld sp;\n",
						fixup->identifier.val.identifier,
						fixup->identifier.offset);
					break;

				default:
					ierror(0);
			}

			fixup = fixup->next;
		}
		fprintf(fp, "];\n");
	}
}

/* The code generator's asking us to pass a parameter in a register. */

int reg_parm(struct reg_handle *rh, struct Typ *typ, int vararg, struct Typ *ft)
{
	/* Vararg parameters never go in registers. */

	if (vararg)
		return 0;

	/* Will the parameter fit? */

	if (sizetab[typ->flags & NQ] > 2)
		return 0;

	/* Still enough registers? */

	if (rh->reg >= NUM_REGPARMS+USERREG)
		return 0;

	return (rh->reg++);
}

int reg_pair(int r,struct rpair *p)
/* Returns 0 if the register is no register pair. If r  */
/* is a register pair non-zero will be returned and the */
/* structure pointed to p will be filled with the two   */
/* elements.                                            */
{
  return 0;
}                                                                               
void init_db(FILE *f)
{
}
void cleanup_db(FILE *f)
{
}
