/*  Example of a code-generator for SAB c16x 16bit microcontrollers.*/

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
#undef BOOL
#undef MAXINT
#undef MAX_TYPE

#define BIT 1
#define CHAR 2
#define SHORT 3
#define INT 4
#define LONG 5
#define LLONG 6
#define FLOAT 7
#define DOUBLE 8
#define LDOUBLE 9
#define VOID 10
#define NPOINTER 11
#define FPOINTER 12
#define HPOINTER 13
#define ARRAY 14
#define STRUCT 15
#define UNION 16
#define ENUM 17
#define FUNKT 18
#define BOOL 19

#define MAXINT 20

#define MAX_TYPE MAXINT

#define POINTER_TYPE(x) pointer_type(x)
extern int pointer_type();
#define ISPOINTER(x) ((x&NQ)>=NPOINTER&&(x&NQ)<=HPOINTER)
#define ISSCALAR(x) ((x&NQ)>=BIT&&(x&NQ)<=HPOINTER)
#define ISINT(x) ((x&NQ)>=BIT&&(x&NQ)<=LLONG)
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

/*  This struct can be used to implement machine-specific           */
/*  addressing-modes.                                               */
/*  Not used in this code-generrator.                               */
struct AddressingMode{
    int flags;
    int base;
    long offset;
    struct Var *v;
};

/* This type will be added to every IC. Can be used by the cg.      */
#define HAVE_EXT_IC 1
struct ext_ic {
  int flags;
  int r;
  long offset;
};

/*  The number of registers of the target machine.                  */
#define MAXR 25

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

#define HAVE_REGPARMS 1

struct reg_handle {
  int gpr;
};

/*  We use unsigned int as size_t rather than unsigned long which   */
/*  is the default setting.                                         */
#define HAVE_INT_SIZET 1

/*  We have register pairs.                                         */
#define HAVE_REGPAIRS 1

/*  We keep track of all registers modified by a function.          */
#define HAVE_REGS_MODIFIED 1

#define HAVE_TARGET_RALLOC 1
#define cost_load_reg(r,v) 4
#define cost_save_reg(r,v) 4
#define cost_move_reg(i,j) 2
#define cost_pushpop_reg(r) 2

/* size of buffer for asm-output */
#define EMIT_BUF_LEN 1024 /* should be enough */
/* number of asm-output lines buffered */
#define EMIT_BUF_DEPTH 4

/*  We have asm_peephole to optimize assembly-output */
#define HAVE_TARGET_PEEPHOLE 1

/*  We have some target-specific variable attributes.               */
#define HAVE_TARGET_ATTRIBUTES 1

/* We use builtin libcalls for some operations */
#define HAVE_LIBCALLS 1
