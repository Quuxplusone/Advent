
void set_indentation(int level);
void indent_appropriately(void);
void puts_indented(const char *text);

#ifndef Z_MACHINE
/* On the Z-machine, this substitution is built into vbccz itself.
 * See the Makefile. */
#define puts puts_indented
#endif /* Z_MACHINE */
