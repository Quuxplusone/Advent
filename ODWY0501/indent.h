
void set_indentation(int level);
void indent_appropriately(void);
void puts_indented(const char *text);

#define puts puts_indented
