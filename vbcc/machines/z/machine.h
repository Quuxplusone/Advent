/* Z-machine code generator
 * David Given
 */

#include "dt.h"

/* Machine specific addressing-modes. Not used. */

struct AddressingMode{
	int never_used;
};

/* The number of registers we support. We don't really have any, but we
 * use local variables instead; we have 14.
 */

#define MAXR 14

/* Number of command-line options we accept. */

#define MAXGF 6

/* If this is set to zero vbcc will not generate ICs where the target operand
 * is the same as the 2nd source operand. This can sometimes simplify the
 * code-generator, but usually the code is better if the code-generator allows
 * it. 
 */

#define USEQ2ASZ 1

/* The smallest and largest integer type that can be added to a pointer. */

#define MINADDI2P INT
#define MAXADDI2P INT

/* Big-endian? */

#define BIGENDIAN 1

/* Little-endian? */

#define LITTLEENDIAN 0

/* If switch-statements should be generated as a sequence of SUB,TST,BEQ ICs
 * rather than COMPARE,BEQ ICs set this to 1.  This can yield better code on
 * some machines.
 */

#define SWITCHSUBS 0

/* In optimizing compilation certain library memcpy/strcpy-calls with length
 * known at compile-time will be inlined using an ASSIGN-IC if the size is less
 * or equal to INLINEMEMCPY.  The type used for the ASSIGN-IC will be
 * UNSIGNED|CHAR. On the Z-machine, memcpy can be done in `hardware' with the
 * @copy_table opcode, so always inline them if possible.
 */

#define INLINEMEMCPY 65536

/* Do we want to pass parameters to functions in registers? */

#define HAVE_REGPARMS 1

/* If so, how many? Max 7 due to the architecture, but one is always xp. */

#define NUM_REGPARMS 6

/* This structure is used to keep track of where register parameters go. */

struct reg_handle {
	int reg;
};

/* Do we want to use zuint for size_t rather than the default zulong? */

#define HAVE_INT_SIZET 1

/* size of buffer for asm-output */
#define EMIT_BUF_LEN 1024 /* should be enough */
/* number of asm-output lines buffered */
#define EMIT_BUF_DEPTH 4
