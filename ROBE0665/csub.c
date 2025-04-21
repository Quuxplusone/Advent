/*
 * File: csub.c
 * ------------
 * This file contains a few C-based functions for the newadv
 * application.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/file.h>
#include "params.h"

#define FILENAME "newadv.sav"

#define common(symbol,name,size) char symbol[(size+3) & -4]
#include "common.h"
#undef common

float crand_() {
    return rand() / ((double) RAND_MAX + 1);
}

void csave_(char cbuf[]) {
    int ochan;

    ochan = open(FILENAME, O_WRONLY|O_CREAT|O_TRUNC, 0777);
    if (ochan == -1) exit(1);

#define common(symbol,name,size) write(ochan, symbol, size)
#include "volatile.h"
#undef common

    close(ochan);
    printf("%s saved\n", FILENAME);
}

void crest_(char cbuf[]) {
    int ochan;

    ochan = open(FILENAME, O_RDONLY, 0);
    if (ochan == -1) {
        printf("Can't open the file %s\n", FILENAME);
        exit(1);
    }

#define common(symbol,name,size) read(ochan, symbol, size)
#include "volatile.h"
#undef common

    close(ochan);
}

void readln_(char cbuf[]) {
    int i, ch, semi;

    semi = -1;
    for (i = 0; ; i++) {
        ch = getchar();
        if (ch == EOF) {
            printf("\n");
            exit(0);
        }
        if (ch == '\r' || ch == '\n') break;
        if (ch == ';') semi = i;
        if (i < BUFMAX - 1) cbuf[i] = ch;
    }
    if (i > BUFMAX - 1) i = BUFMAX - 1;
    cbuf[i] = '\0';
    if (!isatty(0)) printf("%s\n", cbuf);
    if (semi >= 0) i = semi;
    while (i < BUFMAX) cbuf[i++] = ' ';
}
