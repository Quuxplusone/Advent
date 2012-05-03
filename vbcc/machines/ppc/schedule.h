/*
 * vscppc
 *
 * vbcc PowerPC scheduler
 * (C)1998 by Frank Wille <frank@phoenix.owl.de>
 *
 * vscppc is freeware and part of the portable and retargetable ANSI C
 * compiler vbcc, copyright (c) 1995-98 by Volker Barthelmann.
 * vscppc may be freely redistributed as long as no modifications are
 * made and nothing is charged for it. Non-commercial usage is allowed
 * without any restrictions.
 * EVERY PRODUCT OR PROGRAM DERIVED DIRECTLY FROM MY SOURCE MAY NOT BE
 * SOLD COMMERCIALLY WITHOUT PERMISSION FROM THE AUTHOR.
 *
 * History:
 * V0.3   20-Jul-98
 *        Differentiation between 603 and 604. Now, scheduling takes
 *        place with regard to the real PowerPC architecture.
 * V0.1   10-Jul-98
 *        vscppc seems to be stable enough, after some tests.
 *        However, it still needs a lot of fine tuning.
 *        A differentiation between the PPC CPUs (603e, 604e) is missing.
 * V0.0   09-Jul-98
 *        File created.
 *
 */

#define PIPES 7   /* the max. number of pipes, as required by the 604 */

/* Pipe Names 603 */
#define BPU 0     /* Branch Prediction Unit */
#define SRU 1     /* Special Reg. Unit */
#define IU 2      /* Integer Unit */
#define FPU 5     /* Floating Point Unit */
#define LSU 6     /* Load Store Unit */

/* Pipe Names 604 */
#define CRU 1     /* Condition Register Unit */
#define SCIU1 2   /* Single Cycle Integer Unit #1 */
#define SCIU2 3   /* Single Cycle Integer Unit #2 */
#define MCIU 4    /* Multiple Cycle Integer Unit */


#define REGS 76   /* 32 GPR, 32 FPR, 8 CCR, LR, CTR, XER, FPSCR */

/* REG-offsets */
#define GPR 0
#define FPR 32
#define CCR 64
#define XER 72
#define CTR 73
#define LR 74
#define FPSCR 75
