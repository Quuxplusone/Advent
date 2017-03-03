/*  Backend for VideoCore IV
    (c) in 2013 by Volker Barthelmann
*/

/* buil-time configurable options: */
#define NUM_GPRS 48

#include "dt.h"


/*  This struct can be used to implement machine-specific           */
/*  addressing-modes.                                               */
/*  Currently possible are (const,gpr) and (gpr,gpr)                */
struct AddressingMode{
    int flags;
    int base;
    long offset;
};

/*  The number of registers of the target machine.                  */
#define MAXR 48

/*  Number of commandline-options the code-generator accepts.       */
#define MAXGF 20

/*  If this is set to zero vbcc will not generate ICs where the     */
/*  target operand is the same as the 2nd source operand.           */
/*  This can sometimes simplify the code-generator, but usually     */
/*  the code is better if the code-generator allows it.             */
#define USEQ2ASZ 0

/*  This specifies the smallest integer type that can be added to a */
/*  pointer.                                                        */
#define MINADDI2P INT

/*  This specifies the smallest integer type that can be added to a */
/*  pointer.                                                        */
#define MAXADDI2P INT

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
#define SWITCHSUBS 1

/*  In optimizing compilation certain library memcpy/strcpy-calls   */
/*  with length known at compile-time will be inlined using an      */
/*  ASSIGN-IC if the size is less or equal to INLINEMEMCPY.         */
/*  The type used for the ASSIGN-IC will be UNSIGNED|CHAR.          */
#define INLINEMEMCPY 1024

/*  Parameters are sometimes passed in registers without __reg.     */
#define HAVE_REGPARMS 1

/*  Parameters on the stack should be pushed in order rather than   */
/*  in reverse order.                                               */
#undef ORDERED_PUSH

/*  Structure for reg_parm().                                       */
struct reg_handle{
    unsigned long gregs;
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
#define cost_move_reg(x,y) 1
#define cost_load_reg(x,y) 2
#define cost_save_reg(x,y) 2
#define cost_pushpop_reg(x) 3

/* size of buffer for asm-output, this can be used to do
   peephole-optimizations of the generated assembly-output */
#define EMIT_BUF_LEN 1024 /* should be enough */
/* number of asm-output lines buffered */
#define EMIT_BUF_DEPTH 4

/*  We have no asm_peephole to optimize assembly-output */
#define HAVE_TARGET_PEEPHOLE 0

/* we do not have a mark_eff_ics function, this is used to prevent
   optimizations on code which can already be implemented by efficient
   assembly */
#undef HAVE_TARGET_EFF_IC

/* we only need the standard data types (no bit-types, different pointers
   etc.) */
#undef HAVE_EXT_TYPES
#undef HAVE_TGT_PRINTVAL

/* we do not need extra elements in the IC */
#undef HAVE_EXT_IC

/* we do use unsigned int as size_t (but unsigned long, the default) */
#define HAVE_INT_SIZET 1

/* we need register-pairs */
#define HAVE_REGPAIRS 1


/* do not create CONVERT ICs from integers smaller than int to floats */
#define MIN_INT_TO_FLOAT_TYPE INT

/* do not create CONVERT ICs from floats to ints smaller than int */
#define MIN_FLOAT_TO_INT_TYPE INT

/* do not create CONVERT_ICs from floats to unsigned integers */
#define AVOID_FLOAT_TO_UNSIGNED 1

/* do not create CONVERT_ICs from unsigned integers to floats */
#define AVOID_UNSIGNED_TO_FLOAT 0

#define HAVE_LIBCALLS 1
