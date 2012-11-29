
#ifndef H_STDIO
 #define H_STDIO

#ifndef NULL
 #define NULL 0
#endif /* NULL */

typedef struct _Neverdefined FILE;
extern FILE *stdin;
extern FILE *stdout;

void printf(const char *fmt, ...);
void puts(const char *);
void putc(int, FILE *);
void fgets(char *buffer, int len, FILE *in);

void fflush(FILE *) = "";

#endif /* H_STDIO */
