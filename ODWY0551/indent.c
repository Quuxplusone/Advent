
#include <stdio.h>
#include "indent.h"

static int Indentation = 0;
static int ThisLine = 0;

void set_indentation(int level)
{
    Indentation = ThisLine = level;
}

void indent_appropriately()
{
    for (int i=0; i < ThisLine; ++i)
        putchar(' ');
    ThisLine = 0;
}

static void putchar_indented(char ch)
{
    if (ch == '\n') {
        putchar('\n');
        ThisLine = Indentation;
    } else {
        indent_appropriately();
        putchar(ch);
    }
}

void puts_indented(const char *text)
{
    for (int i=0; text[i] != '\0'; ++i) {
        putchar_indented(text[i]);
    }
    putchar_indented('\n');
}
