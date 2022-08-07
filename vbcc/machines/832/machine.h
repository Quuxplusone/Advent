/*  EightThirtyTwo backend for vbcc.
    Based on the "generic" backend.
*/

/* build-time configurable options: */
#define NUM_GPRS 9
#define SCRATCH_GPRS 2
#define NUM_FPRS 0
#define NUM_CCRS 0
#define FIXED_SP 0

#include "dt.h"

/* internally used by the backend */
#define FIRST_GPR 1
#define LAST_GPR (FIRST_GPR+NUM_GPRS-1)
#define FIRST_FPR (LAST_GPR+1)
#define LAST_FPR (FIRST_FPR+NUM_FPRS-1)
#define RESERVED_GPRS 1

/*  This struct can be used to implement machine-specific           */
/*  addressing-modes.                                               */
/*  Currently possible are (const,gpr) and (gpr,gpr)                */
/*  FIXME - we can make use of ldidx here */
enum AddressingMode_Type { AM_POSTINC=1, AM_PREDEC, AM_ADDT };
struct AddressingMode{
    enum AddressingMode_Type type;
	int disposable;
	int deferredpop;
};

/*  The number of registers of the target machine.                  */
#define MAXR NUM_GPRS+NUM_FPRS+NUM_CCRS

/*  Number of commandline-options the code-generator accepts.       */
#define MAXGF 1

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
#define SWITCHSUBS 1

/*  In optimizing compilation certain library memcpy/strcpy-calls   */
/*  with length known at compile-time will be inlined using an      */
/*  ASSIGN-IC if the size is less or equal to INLINEMEMCPY.         */
/*  The type used for the ASSIGN-IC will be UNSIGNED|CHAR.          */
#define INLINEMEMCPY 1024

/*  Parameters are sometimes passed in registers without __reg.     */
#define HAVE_REGPARMS 1
#define REGPARM_COUNT 1

/*  Parameters on the stack should be pushed in order rather than   */
/*  in reverse order.                                               */
// #define ORDERED_PUSH 1
// FIXED_SP

/*  Structure for reg_parm().                                       */
struct reg_handle{
    unsigned long gregs;
    unsigned long fregs;
};

/*  We have some target-specific variable attributes.               */
#define HAVE_TARGET_ATTRIBUTES

/* We have target-specific pragmas */
#define HAVE_TARGET_PRAGMAS

/*  We keep track of all registers modified by a function.          */
#define HAVE_REGS_MODIFIED 1

/* We have implemented our own cost-functions to adapt 
   register-allocation */
#define HAVE_TARGET_RALLOC 1
#define cost_move_reg(x,y) 1
#define cost_load_reg(x,y) 5
#define cost_save_reg(x,y) 5
#define cost_pushpop_reg(x) 1

/* size of buffer for asm-output, this can be used to do
   peephole-optimizations of the generated assembly-output */
#define EMIT_BUF_LEN 1024 /* should be enough */
/* number of asm-output lines buffered */
#define EMIT_BUF_DEPTH 16

/*  We have no asm_peephole to optimize assembly-output */
#define HAVE_TARGET_PEEPHOLE 1

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

/* we use unsigned int as size_t (but unsigned long, the default) */
#define HAVE_INT_SIZET 1

/* we do not need register-pairs */
#undef HAVE_REGPAIRS

/* We (will eventually) use libcalls for 64-bit and float support */
/* Enabling this completely changes code generation - investigate. */
#define HAVE_LIBCALLS 0

/* do not create CONVERT ICs from integers smaller than int to floats */
#define MIN_INT_TO_FLOAT_TYPE INT

/* do not create CONVERT ICs from floats to ints smaller than int */
#define MIN_FLOAT_TO_INT_TYPE INT

/* do not create CONVERT_ICs from floats to unsigned integers */
#define AVOID_FLOAT_TO_UNSIGNED 1

/* do not create CONVERT_ICs from unsigned integers to floats */
#define AVOID_UNSIGNED_TO_FLOAT 1
