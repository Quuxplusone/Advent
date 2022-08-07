/*  $VER: vbcc (m68k/machine.h) $Revision: 1.13 $     */

#include "dt.h"

/*  This struct can be used to implement machine-specific           */
/*  addressing-modes.                                               */
typedef struct AddressingMode{
    int basereg;
    long dist;
    int skal;
    int dreg;
} AddressingMode;

/*  The number of registers of the target machine.                  */
#define MAXR 28

/*  Number of commandline-options the code-generator accepts.       */
#define MAXGF 35

/*  If this is set to zero vbcc will not generate ICs where the     */
/*  target operand is the same as the 2nd source operand.           */
/*  This can sometimes simplify the code-generator, but usually     */
/*  the code is better if the code-generator allows it.             */
#define USEQ2ASZ 1

/*  This specifies the smallest integer type that can be added to a */
/*  pointer.                                                        */
extern int MINADDI2P;

/*  This specifies the smallest unsigned type that can be added to a */
/*  pointer.                                                        */
#define MINADDUI2P (UNSIGNED|INT)


/*  This specifies the biggest integer type that can be added to a  */
/*  pointer.                                                        */
#define MAXADDI2P LONG

/*  If the bytes of an integer are ordered most significant byte    */
/*  byte first and then decreasing set BIGENDIAN to 1.              */
#define BIGENDIAN 1

/*  If the bytes of an integer are ordered lest significant byte    */
/*  byte first and then increasing set LITTLEENDIAN to 1.           */
#define LITTLEENDIAN 0

/*  Note that BIGENDIAN and LITTLEENDIAN are mutually exclusive.    */

/*  If switch-statements should be generated as a sequence of       */
/*  SUB,TST,BEQ ICs rather than COMPARE,BEQ ICs set this to 1.      */
/*  This can yield better code on some machines.                    */
#define SWITCHSUBS 1

/*  In optimizing compilation certain library memcpy/strcpy-calls   */
/*  with length known at compile-time will be inlined using an      */
/*  ASSIGN-IC if the size is less or equal to INLINEMEMCPY.         */
/*  The type used for the ASSIGN-IC will be UNSIGNED|CHAR.          */
#define INLINEMEMCPY 256

/*  We have some target-specific variable attributes.               */
#define HAVE_TARGET_ATTRIBUTES 1

/*  We keep track of all registers modified by a function.          */
#define HAVE_REGS_MODIFIED 1

#define HAVE_TARGET_RALLOC 1
#define cost_move_reg(x,y) 1
#define cost_load_reg(x,y) 2
#define cost_save_reg(x,y) 2
#define cost_pushpop_reg(x) 2

/* size of buffer for asm-output */
#define EMIT_BUF_LEN 1024 /* should be enough */
/* number of asm-output lines buffered */
#define EMIT_BUF_DEPTH 4

/*  We have no asm_peephole to optimize assembly-output */
#define HAVE_TARGET_PEEPHOLE 1

/* we have a mark_eff_ics function */
#define HAVE_TARGET_EFF_IC 1

/* we have register-pairs */
#define HAVE_REGPAIRS 1

#define HAVE_REGPARMS 1

struct reg_handle {
  int dr,ar,fr;
};


#define JUMP_TABLE_DENSITY 0.8
#define JUMP_TABLE_LENGTH 8

/* support for variable-length arrays */
#define ALLOCVLA_REG 9
#define ALLOCVLA_INLINEASM "\taddq.l\t#3,d0\n\tand.b\t#252,d0\n\tsub.l\td0,a7\n\tmove.l\ta7,d0"
#define FREEVLA_REG 0
/* TODO: find a better solution some time */
#define FREEVLA_INLINEASM "\tmove.l\t(a7),a7\n\tsubq.l\t#4,a7"
#define OLDSPVLA_INLINEASM "\tmove.l\ta7,d0"
#define FPVLA_REG 6

/* We use builtin libcalls for some operations */
#define HAVE_LIBCALLS 1

/* We have target-specific pragmas */
#define HAVE_TARGET_PRAGMAS

/* We have a target-specific add_var hook */
#define HAVE_TARGET_VARHOOK_PRE

#define HAVE_POF2OPT 1

#ifndef M68K_16BIT_INT
#define HAVE_INT_SIZET 1
#endif
