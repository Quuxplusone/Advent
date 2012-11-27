
void set_indentation(int level);
void indent_appropriately(void);
void puts_indented(const char *text);

#ifndef Z_MACHINE
/* TODO: make indentation work efficiently on the Z-machine */
#define puts puts_indented
#endif /* Z_MACHINE */