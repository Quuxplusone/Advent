/*  6502 backend for vbcc
    (c) Volker Barthelmann 2020

*/

/* buil-time configurable options: */
#define NUM_GPRS 32
#define NUM_PAIRS (NUM_GPRS/2)
#define NUM_BIG 4
#define NUM_BIGP (NUM_BIG/2)
#define FIXED_SP 1

#include "dt.h"

#undef CHAR
#undef SHORT
#undef INT
#undef LONG
#undef LLONG
#undef FLOAT
#undef DOUBLE
#undef LDOUBLE
#undef VOID
#undef POINTER
#undef ARRAY
#undef STRUCT
#undef UNION
#undef ENUM
#undef FUNKT
#undef BOOL
#undef MAXINT
#undef MAX_TYPE

#define CHAR 1
#define SHORT 2
#define INT 3
#define LONG 4
#define LLONG 5
#define FLOAT 6
#define DOUBLE 7
#define LDOUBLE 8
#define VOID 9
#define POINTER 10
#define FPOINTER 11
#define HPOINTER 12
#define ARRAY 13
#define STRUCT 14
#define UNION 15
#define ENUM 16
#define FUNKT 17
#define BOOL 18

#define MAXINT 19

#define MAX_TYPE MAXINT


#define POINTER_TYPE(x) pointer_type(x)
#define POINTER_VARADR(x) pointer_varadr(x)
extern int pointer_type();
extern int pointer_varadr();
#define ISPOINTER(x) ((x&NQ)>=POINTER&&(x&NQ)<=HPOINTER)
#define ISSCALAR(x) ((x&NQ)>=CHAR&&(x&NQ)<=HPOINTER)
#define ISINT(x) ((x&NQ)>=CHAR&&(x&NQ)<=LLONG)
#define PTRDIFF_T(x) ((x)==HPOINTER?LONG:INT)

typedef zllong zmax;
typedef zullong zumax;

union atyps{
  zchar vchar;
  zuchar vuchar;
  zshort vshort;
  zushort vushort;
  zint vint;
  zuint vuint;
  zlong vlong;
  zulong vulong;
  zllong vllong;
  zullong vullong;
  zmax vmax;
  zumax vumax;
  zfloat vfloat;
  zdouble vdouble;
  zldouble vldouble;
};


/* internally used by the backend */
#define FIRST_GPR 8
#define LAST_GPR (FIRST_GPR+NUM_GPRS-1)
#define FIRST_PAIR (LAST_GPR+1)
#define LAST_PAIR (FIRST_PAIR+NUM_PAIRS-1)
#define FIRST_BIG (LAST_PAIR+1)
#define LAST_BIG (FIRST_BIG+NUM_BIG-1)
#define FIRST_BIGP (LAST_BIG+1)
#define LAST_BIGP (FIRST_BIGP+NUM_BIGP-1)

/*  This struct can be used to implement machine-specific           */
/*  addressing-modes.                                               */
/*  Currently possible are (const,gpr) and (gpr,gpr)                */
struct AddressingMode{
  int flags;
  int base;
  int idx;
  long offset;
  void *v;
};

/*  The number of registers of the target machine.                  */
#define MAXR LAST_BIGP

/*  Number of commandline-options the code-generator accepts.       */
#define MAXGF 30

/*  If this is set to zero vbcc will not generate ICs where the     */
/*  target operand is the same as the 2nd source operand.           */
/*  This can sometimes simplify the code-generator, but usually     */
/*  the code is better if the code-generator allows it.             */
#define USEQ2ASZ 1

/*  This specifies the smallest integer type that can be added to a */
/*  pointer.                                                        */
#define MINADDI2P CHAR

/*  If the bytes of an integer are ordered most significant byte    */
/*  byte first and then decreasing set BIGENDIAN to 1.              */
#define BIGENDIAN 0

/*  If the bytes of an integer are ordered lest significant byte    */
/*  byte first and then increasing set LITTLEENDIAN to 1.           */
#define LITTLEENDIAN 1

/*  Note that BIGENDIAN and LITTLEENDIAN are mutually exclusive.    */

/*  If switch-statements should be generated as a sequence of       */
/*  SUB,TST,BEQ ICs rather than COMPARE,BEQ ICs set this to 1.      */
/*  This can yield better code on some machines.                    */
#define SWITCHSUBS 0

/*  In optimizing compilation certain library memcpy/strcpy-calls   */
/*  with length known at compile-time will be inlined using an      */
/*  ASSIGN-IC if the size is less or equal to INLINEMEMCPY.         */
/*  The type used for the ASSIGN-IC will be UNSIGNED|CHAR.          */
#define INLINEMEMCPY 1024

/*  Parameters are sometimes passed in registers without __reg.     */
#define HAVE_REGPARMS 1

/*  Parameters on the stack should be pushed in order rather than   */
/*  in reverse order.                                               */
#define ORDERED_PUSH FIXED_SP

/*  Structure for reg_parm().                                       */
struct reg_handle{
  int regs;
  int bregs;
};

/*  We have some target-specific variable attributes.               */
#define HAVE_TARGET_ATTRIBUTES

/* We have target-specific pragmas */
#define HAVE_TARGET_PRAGMAS

/*  We keep track of all registers modified by a function.          */
#define HAVE_REGS_MODIFIED 1

/* We have a implement our own cost-functions to adapt 
   register-allocation */
#define HAVE_TARGET_RALLOC 1
#define cost_move_reg(x,y) 6
#define cost_load_reg(x,y) 10
#define cost_save_reg(x,y) 10
#define cost_pushpop_reg(x) 12

/* size of buffer for asm-output, this can be used to do
   peephole-optimizations of the generated assembly-output */
#define EMIT_BUF_LEN 1024 /* should be enough */
/* number of asm-output lines buffered */
#define EMIT_BUF_DEPTH 8

/*  We have no asm_peephole to optimize assembly-output */
#define HAVE_TARGET_PEEPHOLE 1

/* we do not have a mark_eff_ics function, this is used to prevent
   optimizations on code which can already be implemented by efficient
   assembly */
#undef HAVE_TARGET_EFF_IC

/* we have additional types */
#define HAVE_EXT_TYPES
#define HAVE_TGT_PRINTVAL

/* we do not need extra elements in the IC */
#undef HAVE_EXT_IC

/* we do not use unsigned int as size_t (but unsigned long, the default) */
#define HAVE_INT_SIZET 1

/* we do not need register-pairs */
#define HAVE_REGPAIRS 1


/* do not create CONVERT ICs from integers smaller than int to floats */
#define MIN_INT_TO_FLOAT_TYPE INT

/* do not create CONVERT ICs from floats to ints smaller than int */
#define MIN_FLOAT_TO_INT_TYPE INT

/* do not create CONVERT_ICs from floats to unsigned integers */
#define AVOID_FLOAT_TO_UNSIGNED 0

/* do not create CONVERT_ICs from unsigned integers to floats */
#define AVOID_UNSIGNED_TO_FLOAT 0

/* convert multiplications/division by powers of two to shifts */
#define HAVE_POF2OPT 1

/* We use builtin libcalls for some operations */
#define HAVE_LIBCALLS 1

/* Use char for return of comparison libcalls */
#define LIBCALL_CMPTYPE CHAR

/* We prefer BNE rather than BGT. */
#define HAVE_WANTBNE 1

#define BESTCOPYT CHAR

#define HAVE_AOS4 1

#define CHARCONV(x) cbmconv(x)
unsigned char cbmconv(unsigned char);

#define ALLOCVLA_REG FIRST_PAIR
#define ALLOCVLA_INLINEASM "\tlda\tsp\n"\
                           "\tsec\n"\
                           "\tsbc\tr0\n"\
                           "\tsta\tsp\n"\
                           "\tlda\tsp+1\n"\
                           "\tsbc\tr1\n"\
                           "\tsta\tsp+1\n"\
                           "\tlda\tsp\n"\
                           "\tclc\n"\
                           "\tldx\tsp+1\n"\
                           "\tadc\t#___fo\n"\
                           "\tbcc\t*+3\n"\
                           "\tinx\n"

#define FREEVLA_REG FIRST_PAIR
#define FREEVLA_INLINEASM "\tlda\tr0\n"\
                          "\tsta\tsp\n"\
                          "\tlda\tr1\n"\
                          "\tsta\tsp+1\n"

#define OLDSPVLA_INLINEASM "\tlda\tsp+1\n"\
                           "\ttax\n"\
                           "\tlda\tsp"

#define FPVLA_REG (LAST_PAIR-2)

#define HAVE_TARGET_VARHOOK_POST 1

#define HAVE_DECIDE_REVERSE 1

#define HAVE_TARGET_EFF_IC 1
