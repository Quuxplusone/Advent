/*
 * Memory manipulation routines
 * (c) Thomas Pornin 1998, 1999, 2000
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 4. The name of the authors may not be used to endorse or promote
 *    products derived from this software without specific prior written
 *    permission.
 *
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR 
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHORS OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR 
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE 
 * OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

#include "mem.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef AUDIT
void die(void)
{
	abort();
}

static void suicide(unsigned long e)
{
	fprintf(stderr, "ouch: Schrodinger's beef is not dead ! %lx\n", e);
	die();
}
#else
void die(void)
{
	exit(1);
}
#endif

#if defined AUDIT || defined MEM_CHECK
/*
 * This function is equivalent to a malloc(), but will display an error
 * message and exit if the wanted memory is not available
 */
void *getmem(size_t x)
{
	void *m;

#ifdef AUDIT
	m = malloc(x + 8);
#else
	m = malloc(x);
#endif
	if (m == 0) {
		fprintf(stderr, "ouch: malloc() failed\n");
		die();
	}
#ifdef AUDIT
	*((unsigned long *)m) = 0xdeadbeefUL;
	return (void *)(((char *)m) + 8);
#else
	return m;
#endif
}
#endif

/*
 * This function is equivalent to a realloc(); if the realloc() call
 * fails, it will try a malloc() and a memcpy(). If not enough memory is
 * available, the program exits with an error message
 */
void *incmem(void *m, size_t x, size_t nx)
{
	void *nm;

#ifdef AUDIT
	m = (void *)(((char *)m) - 8);
	if (*((unsigned long *)m) != 0xdeadbeefUL)
		suicide(*((unsigned long *)m));
	x += 8; nx += 8;
#endif
	if (!(nm = realloc(m, nx))) {
		if (x > nx) x = nx;
		nm = getmem(nx);
		memcpy(nm, m, x);
		/* free() and not freemem(), because of the Schrodinger beef */
		free(m);
	}
#ifdef AUDIT
	return (void *)(((char *)nm) + 8);
#else
	return nm;
#endif
}

#ifdef AUDIT
/*
 * This function frees the given block
 */
void freemem(void *x)
{
#ifdef AUDIT
	void *y = (void *)(((char *)x) - 8);

	if ((*((unsigned long *)y)) != 0xdeadbeefUL)
		suicide(*((unsigned long *)y));
	*((unsigned long *)y) = 0xfeedbabeUL;
	free(y);
#else
	free(x);
#endif
}
#endif

#ifdef AUDIT
/*
 * This function copies n bytes from src to dest
 */
void *mmv(void *dest, void *src, size_t n)
{
	return memcpy(dest, src, n);
}

/*
 * This function copies n bytes from src to dest
 */
void *mmvwo(void *dest, void *src, size_t n)
{
	return memmove(dest, src, n);
}
#endif

/*
 * This function creates a new char * and fills it with a copy of src
 */
char *sdup(char *src)
{
	size_t n = 1 + strlen(src);
	char *x = getmem(n);

	mmv(x, src, n);
	return x;
}
