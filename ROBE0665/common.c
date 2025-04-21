#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BYTES_PER_LINE 10
#define ALIGNMENT 16

#define common(symbol,name,size) \
    char symbol[((size + ALIGNMENT - 1) & -ALIGNMENT)]
#include "common.h"
#undef common

static FILE *outfile;

/* Local function declarations */

static void DumpHeader(void);
static void DumpData(char *addr, char *name, int size);
static void DumpTrailer(void);

void cdump_() {
    outfile = fopen("data.c", "w");
    if (outfile == NULL) exit(1);
    DumpHeader();

#define common(symbol,name,size) DumpData(symbol, name, size)
#include "common.h"
#undef common

    DumpTrailer();
    fclose(outfile);
}

static void DumpHeader(void) {
    fprintf(outfile, "/* Initialized common structures */\n");
}

static void DumpData(char *addr, char *name, int size) {
    int i, padsize;

    padsize = (size + ALIGNMENT - 1) & -ALIGNMENT;
    fprintf(outfile, "\nchar %s[] = {\n", name);
    for (i = 0; i < size; i++) {
        if (i % BYTES_PER_LINE == 0) fprintf(outfile, "    ");
        fprintf(outfile, "0x%02X, ", (i < size) ? addr[i] & 0xFF : 0);
        if ((i + 1) % BYTES_PER_LINE == 0) fprintf(outfile, "\n");
    }
    if (i % BYTES_PER_LINE != 0) fprintf(outfile, "\n");
    fprintf(outfile, "};\n");
}

static void DumpTrailer(void) {
    /* Empty */
}
