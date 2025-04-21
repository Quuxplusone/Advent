#include <stdio.h>

void readln_(char cbuf[]) {
    int i, ch;

    for (i = 0; ; i++) {
        ch = getchar();
        if (ch == '\r' || ch == '\n') break;
        cbuf[i] = ch;
    }
    while (i < 70) cbuf[i++] = ' ';
}
