/*  Example of a code-generator for a PowerPC                       */

#include "dt.h"

/*  This struct can be used to implement machine-specific           */
/*  addressing-modes.                                               */
/*  Not used in this code-generrator.                               */
struct AddressingMode{
    int flags;
    int base;
    long offset;
};

/*  The number of registers of the target machine.                  */
#define MAXR 87

/*  Number of commandline-options the code-generator accepts.       */
#define MAXGF 30

/*  If this is set to zero vbcc will not generate ICs where the     */
/*  target operand is the same as the 2nd source operand.           */
/*  This can sometimes simplify the code-generator, but usually     */
/*  the code is better if the code-generator allows it.             */
#define USEQ2ASZ 1

/*  This specifies the smallest integer type that can be added to a */
/*  pointer.                                                        */
#define MINADDI2P INT

/*  This specifies the largest integer type that can be added to a  */
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
#define SWITCHSUBS 0

/*  In optimizing compilation certain library memcpy/strcpy-calls   */
/*  with length known at compile-time will be inlined using an      */
/*  ASSIGN-IC if the size is less or equal to INLINEMEMCPY.         */
/*  The type used for the ASSIGN-IC will be UNSIGNED|CHAR.          */
#define INLINEMEMCPY 256

/*  Parameters are sometimes passed in registers without __reg.     */
#define HAVE_REGPARMS 1

/*  Parameters on the stack should be pushed in order rather than   */
/*  in reverse order.                                               */
#define ORDERED_PUSH 1

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

#define HAVE_TARGET_RALLOC 1
#define cost_move_reg(x,y) 1
#define cost_load_reg(x,y) 2
#define cost_save_reg(x,y) 2
#define cost_pushpop_reg(x) 3

/* size of buffer for asm-output */
#define EMIT_BUF_LEN 1024 /* should be enough */
/* number of asm-output lines buffered */
#define EMIT_BUF_DEPTH 4

/*  We have no asm_peephole to optimize assembly-output */
#define HAVE_TARGET_PEEPHOLE 0

/* we have a mark_eff_ics function */
#define HAVE_TARGET_EFF_IC 1

/* we have register-pairs */
#define HAVE_REGPAIRS 1

#define JUMP_TABLE_DENSITY 0.8
#define JUMP_TABLE_LENGTH 12

/* This type will be added to every IC. Can be used by the cg.      */
#define HAVE_EXT_IC 1
struct ext_ic {
  int setcc;
};

/* OSEK support */
#define HAVE_OSEK 1

/* We use builtin libcalls for some operations */
#define HAVE_LIBCALLS 1

/* support for variable-length arrays */
#define ALLOCVLA_REG 4
#if 0
#define ALLOCVLA_INLINEASM "\tlwz\t0,0(1)\n"\
                           "\taddi\t3,3,7\n"\
                           "\tsrwi\t3,3,3\n"\
                           "\tslwi\t3,3,3\n"\
                           "\tneg\t11,3\n"\
                           "\tsub\t3,1,3\n"\
                           "\tsubi\t11,11,8\n"\
                           "\tstwux\t0,1,11"
#endif
#define ALLOCVLA_INLINEASM "\tlwz\t0,0(1)\n"\
                           "\tneg\t3,3\n"\
                           "\tsrwi\t3,3,3\n"\
                           "\tslwi\t3,3,3\n"\
                           "\tstwux\t0,1,3\n"\
                           "\taddi\t3,1,____fo"
#define FREEVLA_REG 6
#define FREEVLA_INLINEASM "\tlwz\t0,0(1)\n"\
                          "\tmr\t1,5\n"\
                          "\tstw\t0,0(1)"
#define OLDSPVLA_INLINEASM "\tmr\t3,1"
#define FPVLA_REG 32
