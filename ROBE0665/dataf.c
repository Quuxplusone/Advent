#include <stdio.h>
#include <ctype.h>
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
    outfile = fopen("data.F", "w");
    if (outfile == NULL) exit(1);
    DumpHeader();

#define common(symbol,name,size) DumpData(symbol, name, size)
#include "common.h"
#undef common

    DumpTrailer();
    fclose(outfile);
}

static void DumpHeader(void) {
    fprintf(outfile, "C       INITIALIZED DATA STRUCTURES\n");
    fprintf(outfile, "        SUBROUTINE DATA()\n");
}

static void DumpData(char *addr, char *name, int size) {
    int i, namelen;
    char *ucname;

    namelen = strlen(name) - 1;
    ucname = malloc(namelen + 1);
    for (i = 0; i < namelen; i++) {
        ucname[i] = toupper(name[i]);
    }
    ucname[namelen] = '\0';
    fprintf(outfile, "\n        INTEGER*1 %s\n", ucname);
    fprintf(outfile, "        COMMON /%s/ %s(%d)\n", ucname, ucname, size);
    fprintf(outfile, "        DATA %s /", ucname);
    for (i = 0; i < size; i++) {
        if (i > 0) fprintf(outfile, ", ");
        if (i % BYTES_PER_LINE == 0) fprintf(outfile, "\n     +    ");
        fprintf(outfile, "%d", addr[i] & 0xFF);
    }
    fprintf(outfile, "/\n");
}

static void DumpTrailer(void) {
    fprintf(outfile, "\n        END\n");
}
