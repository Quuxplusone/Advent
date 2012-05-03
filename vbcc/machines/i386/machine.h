/*  Example of a code-generator for an Intel 386 or higher.         */

#include "dt.h"

/* We have extended types! What we have to do to support them:      */
/* - #define HAVE_EXT_TYPES
   - #undef all standard types
   - #define all standard types plus new types
   - write eval_const and insert_const
   - write typedefs for zmax and zumax
   - write typname[]
   - write conv_typ()
   - optionally #define ISPOINTER, ISARITH, ISINT etc.
   - optionally #define HAVE_TGT_PRINTVAL and write printval
   - optionally #define POINTER_TYPE
   - optionally #define HAVE_TGT_FALIGN and write falign
   - optionally #define HAVE_TGT_SZOF and write szof
   - optionally add functions for attribute-handling
*/
#define HAVE_EXT_TYPES 1

#define HAVE_TGT_PRINTVAL

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
#undef MAXINT
#undef MAX_TYPE

#define CHAR 1
#define SHORT 2
#define BSHORT 3
#define INT 4
#define BINT 5
#define LONG 6
#define BLONG 7
#define LLONG 8
#define BLLONG 9
#define FLOAT 10
#define BFLOAT 11
#define DOUBLE 12
#define BDOUBLE 13
#define LDOUBLE 14
#define BLDOUBLE 15
#define VOID 16
#define POINTER 17
#define BPOINTER 18
#define ARRAY 19
#define STRUCT 20
#define UNION 21
#define ENUM 22
#define FUNKT 23
 
#define MAXINT 24 /* should not be accesible to application */
 
#define MAX_TYPE MAXINT

#define ISPOINTER(x) ((x&NQ)==POINTER||(x&NQ)==BPOINTER)
#define ISSCALAR(x) ((x&NQ)>=CHAR&&(x&NQ)<=BPOINTER)
#define ISINT(x) ((x&NQ)>=CHAR&&(x&NQ)<=BLLONG)

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

/*  This struct can be used to implement machine-specific           */
/*  addressing-modes.                                               */
/*  Not used in this code-generrator.                               */
struct AddressingMode{
  int mode;
  int base;
  int idx;
  int scal;
  zmax offset;
  struct Var *v;
};

/*  The number of registers of the target machine.                  */
#define MAXR 18

/*  Number of commandline-options the code-generator accepts.       */
#define MAXGF 10

/*  If this is set to zero vbcc will not generate ICs where the     */
/*  target operand is the same as the 2nd source operand.           */
/*  This can sometimes simplify the code-generator, but usually     */
/*  the code is better if the code-generator allows it.             */
#define USEQ2ASZ 1

/*  This specifies the smallest integer type that can be added to a */
/*  pointer.                                                        */
#define MINADDI2P INT
#define MAXADDI2P LONG

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

/*  We keep track of all registers modified by a function.          */
#define HAVE_REGS_MODIFIED 1

/* size of buffer for asm-output */
#define EMIT_BUF_LEN 1024 /* should be enough */
/* number of asm-output lines buffered */
#define EMIT_BUF_DEPTH 4

/*  We have no asm_peephole to optimize assembly-output */
#define HAVE_TARGET_PEEPHOLE 0

#define JUMP_TABLE_DENSITY 0.8
#define JUMP_TABLE_LENGTH 4

/* We use builtin libcalls for some operations */
#define HAVE_LIBCALLS 1

/* support for variable-length arrays */
#define ALLOCVLA_REG 1
#define ALLOCVLA_INLINEASM "\tsubl\t%eax,%esp\n\tandl\t$-5,%esp\n\tmovl\t%esp,%eax"
/* TODO: find a better solution some time */
#define FREEVLA_REG 0
#define FREEVLA_INLINEASM "\tmovl\t(%esp),%esp\n\tsubl\t$4,%esp"
#define OLDSPVLA_INLINEASM "\tmovl\t%esp,%eax"
#define FPVLA_REG 7

/* do not create CONVERT_ICs from floats to unsigned integers */
#define AVOID_FLOAT_TO_UNSIGNED 1

/* do not create CONVERT_ICs from unsigned integers to floats */
#define AVOID_UNSIGNED_TO_FLOAT 1
