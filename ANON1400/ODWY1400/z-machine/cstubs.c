/*
! This file contains only those (pieces of) standard C library functions
! which are required in order to build "advent3.c" for the Z-machine, and
! are not more easily written in Inform. These functions are:
!
!     void puts(buf)
!     void printf(fmt, ...)
!     void strcpy(dest, src)
!     void strncpy(dest, src, n)
!     int strcmp(a, b)
!     int strncmp(a, b, n)
!     char *strchr(s, c)
!     int atoi(s)
!
! The following functions are defined in Inform, in the file stubs.inf:
!
!     void fgets(buf, size, stdin)
!     void putc(ch, stdout)
!     void exit(0)
*/

#include <stdio.h>   /* puts, printf */
#include <stdlib.h>  /* atoi */
#include <string.h>  /* strcpy, strncmp */
#include <stdarg.h>  /* va_list */

/* These should have unique values, but that's about it. */
FILE *stdin = (FILE *)&stdin;
FILE *stdout = (FILE *)&stdout;

void puts(const char *s)
{
    while (*s) {
        putc(*s, stdout);
        ++s;
    }
    putc('\n', stdout);
}

void strcpy(char *dst, const char *src)
{
    while (*src) {
        *dst++ = *src++;
    }
    *dst = '\0';
}

void strncpy(char *dst, const char *src, int n)
{
    for (int i = 0; i < n; ++i) {
        dst[i] = (*src ? *src++ : '\0');
    }
}

int strcmp(const char *a, const char *b)
{
    while (1) {
        unsigned char ca = *a++, cb = *b++;
        if (ca != cb) return (ca < cb) ? -1 : (ca > cb);
        if (ca == '\0') return 0;
    }
}

int strncmp(const char *a, const char *b, int n)
{
    for (int i=0; i < n; ++i) {
        unsigned char ca = *a++, cb = *b++;
        if (ca != cb) return (ca < cb) ? -1 : (ca > cb);
        if (ca == '\0') return 0;
    }
    return 0;
}

void _print_int(int i) = "\t@print_num r0;\n";

void printf(const char *format, ...)
{
    char c;
    va_list ap;
    va_start(ap, format);
    while (c = *format++) {
        if (c == '%') {
            switch (c = *format++) {
                case 'd': {
                    int i = va_arg(ap, int);
                    _print_int(i);
                    break;
                }
                case 's': {
                    char *s = va_arg(ap, char*);
                    while (c = *s++)
                        putc(c, stdout);
                    break;
                }
                default: {
                    putc(c, stdout);
                    break;
                }
            }
        } else {
            putc(c, stdout);
        }
    }
    va_end(ap);
}

char *strchr(const char *s, char c)
{
    char *p = (char *)s;
    while (1) {
        if (*p == c) return p;
        if (*p == '\0') return NULL;
        ++p;
    }
}

/* We don't handle signed inputs, but that's fine for our purpose. */
int atoi(const char *str)
{
    int result = 0;
    while ('0' <= *str && *str <= '9') {
        result *= 10;
        result += (*str - '0');
        ++str;
    }
    return result;
}
